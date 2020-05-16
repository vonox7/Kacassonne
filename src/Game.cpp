//
// Created by ts on 28.03.20.
//

#include <Game.h>

#include "Game.h"


Game::Game(Connection* connection, int card_number)
  : stack_(card_number), connection_(connection)
{
  if(stack_.getLeftCards() < 1)
    return;
  {
    std::lock_guard<std::mutex> lock(data_lock_);
    for(unsigned i = 0; i < connection_->players_.size(); i++){
      players_.push_back(Player(int(connection_->players_[i].color_), connection_->players_[i].name_));
      if(connection_->player_id_ == connection_->players_[i].id_)
        connection_->player_number_ = i;
    }
    if(connection_->iAmHost())
      connection_->send("next", getAsMessage());
    current_card_ = stack_.next();
  }
  moveCard(0, 0, true);
  layCard(true);
  receiver_ = new std::thread(&Game::recv, this);
}

Game::Game(Connection* connection, const Message& reconnect_reply)
  : stack_(1), connection_(connection)
{
  for(unsigned i = 0; i < connection_->players_.size(); i++){
    players_.push_back(Player(int(connection_->players_[i].color_), connection_->players_[i].name_));
    if(connection_->player_id_ == connection_->players_[i].id_)
      connection_->player_number_ = i;
  }
  updateFromMessage(reconnect_reply);
  receiver_ = new std::thread(&Game::recv, this);
}

bool Game::moveCard(double x, double y, bool force){
  std::lock_guard<std::mutex> lock(data_lock_);
  last_mouse_pos_ = wxPoint2DDouble(x, y);
  if((current_player_ == connection_->player_number_ && current_card_) || force){
    x = std::floor(x);
    y = std::floor(y);
    if(x != current_card_->x() || y != current_card_->y()){
      current_card_->setPosition(x, y);
      sendUpdate("moveCard", x, y);
      return true;
    }
  }
  return false;
}

bool Game::rotateCard(){
  std::lock_guard<std::mutex> lock(data_lock_);
  if(current_player_ == connection_->player_number_ && current_card_){
    current_card_->rotate();
    sendUpdate("rotateCard");
    return true;
  }
  return false;
}

bool Game::flipCard(){
  std::lock_guard<std::mutex> lock(data_lock_);
  if(current_player_ == connection_->player_number_ && current_card_){
    current_card_->flip();
    sendUpdate("flipCard");
    return true;
  }
  return false;
}

bool Game::layCard(bool force){
  std::lock_guard<std::mutex> lock(data_lock_);
  if((current_player_ == connection_->player_number_ && current_card_&& validPosition()) || force){
    current_card_ = nullptr;
    played_cards_.push_back(stack_.get());
    sendUpdate("layCard", played_cards_.back().x(), played_cards_.back().y());
    return true;
  }
  return false;
}

bool Game::doMoveStone(double x, double y, int player_number, bool send){
  for(auto &p : players_){
    for(auto &s : p.stones_){
      if(s.isThere(x, y)){
        s.x_ = Card::OUTSIDE;
        s.y_ = Card::OUTSIDE;
        if(send)
          sendUpdate("moveStone", x, y, player_number);
        return true;
      }
    }
  }
  for(auto &s : players_[player_number].stones_){
    if(s.x_ == Card::OUTSIDE){
      s.x_ = x;
      s.y_ = y;
      if(send)
        sendUpdate("moveStone", x, y, player_number);
      return true;
    }
  }
  return false;
}

bool Game::moveStone(double x, double y, bool any_player){
  bool no_stones;
  {
    std::lock_guard<std::mutex> lock(data_lock_);
    if (any_player || !(current_player_ == connection_->player_number_ && current_card_)) {
      no_stones = !doMoveStone(x, y, connection_->player_number_);
    }
  }
  return !no_stones;
}

bool Game::next(){
  {
    std::lock_guard<std::mutex> lock(data_lock_);
    if(current_player_ == connection_->player_number_ && !current_card_ && stack_.getLeftCards() > 0){
      current_player_ = (current_player_ + 1) % int(players_.size());
      connection_->send("next", getAsMessage());
      current_card_ = stack_.next();
      if(current_player_ == 0)
        update_old_pts_ = true;
      return true;
    }
  }
  return false;
}

bool Game::revert(){
  std::lock_guard<std::mutex> lock(data_lock_);
  if(current_player_ == connection_->player_number_){
    played_cards_.back().setPosition(Card::OUTSIDE, Card::OUTSIDE);
    stack_.push(played_cards_.back());
    played_cards_.pop_back();
    if(current_card_ != nullptr)
      current_player_ = (current_player_ + int(players_.size()) - 1) % int(players_.size());
    current_card_ = stack_.next();
    connection_->send("next", getAsMessage());
    return true;
  }
  return false;
}

bool Game::shuffle(){
  {
    std::lock_guard<std::mutex> lock(data_lock_);
    if(current_player_ == connection_->player_number_ && current_card_){
      stack_.shuffle();
      connection_->send("next", getAsMessage());
      current_card_ = stack_.next();
      return true;
    }
  }
  return false;
}

void Game::flare(const wxPoint2DDouble& pos, bool any_player){
  std::lock_guard<std::mutex> lock(data_lock_);
  if (any_player || (current_player_ != connection_->player_number_ || !current_card_)) {
    flares_.emplace_back(Flare(pos.m_x, pos.m_y, connection_->player_number_));
    sendUpdate("flare", pos.m_x, pos.m_y, connection_->player_number_);
  }
}

void Game::setPoints(int player, int points, bool add, bool send){
  std::lock_guard<std::mutex> lock(data_lock_);
  if(player < int(players_.size())){
    players_[player].points_ = (add ? players_[player].points_ + points : points);
    if(send){
      Message msg;
      msg["type"] = "points";
      msg["idx"] = player;
      msg["points"] = players_[player].points_;
      connection_->send("update", msg);
    }
  }
}

int Game::getPreviewCard() {
  std::lock_guard<std::mutex> lock(data_lock_);
  int card_nr = (unsigned(connection_->player_number_) + players_.size() - unsigned(current_player_)) % players_.size();
  card_nr -= (current_card_ ? 0 : 1);
  if (card_nr < 0 || card_nr >= stack_.getLeftCards())
    return -1;
  else if (stack_.next(card_nr))
    return stack_.next(card_nr)->imageNr();
  else
    return -1;
}

Card::Side getSide(const std::array<Card::Side, 4>& sides, int r, bool flipped, int i){
  int tmp = (flipped ? -r-i : r+i) % 4;
  tmp = (tmp < 0 ? tmp+4 : tmp);
  return sides[tmp];
}

bool Game::validPosition(){
  if(played_cards_.empty())
    return true;

  std::array<Card::Side, 4> curr_sides;
  for(int i=0; i<4; i++){
    curr_sides[i] = getSide(Card::CARD_SIDES[current_card_->imageNr()], current_card_->r(), current_card_->flipped(), i);
  }
  static std::array<wxPoint, 4> offsets = {wxPoint(0, -1), wxPoint(-1, 0), wxPoint(0, 1), wxPoint(1, 0)};
  bool has_neighbor = false;
  for(const auto &card : played_cards_){
    if(current_card_->pt() == card.pt())
      return false;

    for(int i=0; i<4; i++){
      if(current_card_->pt() + offsets[i] == card.pt()){
        if(curr_sides[i] != getSide(Card::CARD_SIDES[card.imageNr()], card.r(), card.flipped(), i + 2))
          return false;
        has_neighbor = true;
      }
    }
  }
  return has_neighbor;
}

bool Game::sendUpdate(const std::string& type, double x, double y, int idx){
  Message msg;
  msg["type"] = type;
  msg["x"] = x;
  msg["y"] = y;
  msg["idx"] = idx;
  connection_->send("update", msg);
  return true;
}

Message Game::getAsMessage() const{
  Message msg;
  msg["stack"] = stack_;
  msg["played_cards"] = played_cards_;
  msg["game_players"] = players_;
  msg["current_player"] = current_player_;
  return msg;
}

void Game::updateFromMessage(const Message& msg){
  {
    std::lock_guard<std::mutex> lock(data_lock_);
    msg.at("stack").get_to(stack_);
    msg.at("played_cards").get_to(played_cards_);
    msg.at("game_players").get_to(players_);
    msg.at("current_player").get_to(current_player_);
    current_card_ = stack_.next();
  }
  moveCard(last_mouse_pos_.m_x, last_mouse_pos_.m_y);
  update_table_ = true;
}

void Game::recv(){
  wxMilliSleep(1000);
  while(running_){
    try{
      auto[t, m] = connection_->recv();
      if(t == "update" && m["type"] == "points")
        setPoints(m["idx"], m["points"], false, false);
      else if(t == "update"){
        std::lock_guard<std::mutex> lock(data_lock_);
        if(current_card_){
          if(m["type"] == "moveCard")
            current_card_->setPosition(m["x"], m["y"]);
          else if(m["type"] == "rotateCard")
            current_card_->rotate();
          else if(m["type"] == "flipCard")
            current_card_->flip();
          else if(m["type"] == "layCard"){
            current_card_ = nullptr;
            played_cards_.push_back(stack_.get());
          }
        }
        if(m["type"] == "moveStone" && m["idx"].get<int>() < int(players_.size()))
          doMoveStone(m["x"], m["y"], m["idx"], false);
        else if(m["type"] == "flare" && m["idx"].get<int>() < int(players_.size())){
          flares_.push_back(Flare(m["x"], m["y"], m["idx"]));
        }
        update_table_ = true;
      }
      else if(t == "next"){
        updateFromMessage(m);
        if(current_player_ == 0)
          update_old_pts_ = true;
        update_table_ = true;
      }
      else if(t == "reconnect_request"){
        std::lock_guard<std::mutex> lock(data_lock_);
        connection_->send("reconnect_reply", getAsMessage());
      }
    }
    catch(std::exception& e){
      std::cout << e.what() << std::endl;
    }
  }
}