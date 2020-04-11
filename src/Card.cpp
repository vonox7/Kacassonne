//
// Created by ts on 28.03.20.
//
#include "Card.h"
#include <fstream>
#include <tuple>
#include <algorithm>
#include <filesystem>


std::string Card::CARD_FOLDER = "./";
std::vector<std::pair<wxImage, int>> Card::CARD_IMAGES;
int Card::CARD_IMAGES_SIZE = 48;


bool Card::initCardImages(){
  if(CARD_IMAGES.empty()){
    const char* env = std::getenv("CARD_FOLDER");
    if(env)
      CARD_FOLDER = env;
    else
      CARD_FOLDER = "data/";
    if(CARD_FOLDER.back() != '/')
      CARD_FOLDER += "/";

    std::vector<std::pair<std::string, int>> card_files;
    for (auto& p : std::filesystem::directory_iterator(CARD_FOLDER)) {
      if (p.path().extension().string() == ".jpg" || p.path().extension().string() == ".png") {
        std::string fn = p.path().filename().string();
        if(fn.find("_") != std::string::npos){
          card_files.push_back(std::make_pair(fn, std::atoi((fn.substr(fn.find("_") + 1, fn.find("."))).c_str())));
          if(fn.substr(0, fn.find("_")) == "WWFWS" || fn.substr(0, fn.find("_")) == "WFWSW" ||
             fn.substr(0, fn.find("_")) == "WWSWF" || fn.substr(0, fn.find("_")) == "WSWFW")
            std::swap(card_files.back(), card_files.front());
        }
      }
    }

    wxInitAllImageHandlers();
    for(auto cf : card_files){
      CARD_IMAGES.push_back({wxImage(CARD_FOLDER + cf.first), cf.second});
    }
  }
  return CARD_IMAGES.size();
}

void Card::paint(wxAutoBufferedPaintDC& dc, const wxPoint& pos, double scale, bool current, bool valid) const {
  int edge_length = cardSize(scale);
  int border = std::min(std::max(1, edge_length/48), 2);
  wxImage tmp = Card::CARD_IMAGES[image_nr_].first.Scale(edge_length - 2*border, edge_length - 2*border);
  switch(r_ % 4){
    case 1: tmp = tmp.Rotate90(); break;
    case 2: tmp = tmp.Rotate180(); break;
    case 3: tmp = tmp.Rotate90(false); break;
  }
  if(current && valid){
    dc.SetBrush(*wxGREEN_BRUSH);
    dc.SetPen(*wxGREEN_PEN);
    dc.DrawRectangle(pos - wxPoint(border, border), wxSize(edge_length + 2*border, edge_length + 2*border));
  }
  dc.DrawBitmap(wxBitmap(tmp), pos + wxPoint(border, border));
  if(!valid){
    wxPen invalid_pen(wxColor(0xFF, 0x00, 0x00, 0), 3*scale, wxPENSTYLE_SOLID);
    dc.SetPen(invalid_pen);
    dc.DrawLine(pos, pos + wxPoint(edge_length, edge_length));
  }
}