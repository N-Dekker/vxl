//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// .NAME vgui_tview
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 12 Oct 99
//
//-----------------------------------------------------------------------------

#include <vcl_string.h>
#include "vgui_tview.h"

#include <vcl_cmath.h>
#include <vcl_iostream.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_slot.h>
#include <vgui/vgui_deck.h>
#include <vgui/vgui_composite.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_listmanager2D.h>
#include <vgui/vgui_popup_params.h>
#include <vgui/vgui_menu.h>

static bool debug = false;


vcl_string vgui_tview::type_name() const {return "vgui_tview";}


vgui_tview::vgui_tview(vgui_tableau_sptr const& t)
  : vgui_wrapper_tableau(t)
  , spacing(10)
  , icon_height(2)
  , icon_width(2)
  , active_icon(0)
  , closest_icon(0)
{
}


vgui_tview::~vgui_tview()
{
}


static void draw_rect(float x, float y, float  w, float h) {
  glBegin(GL_POLYGON);
  glVertex2f(x-w,y-h);
  glVertex2f(x+w,y-h);
  glVertex2f(x+w,y+h);
  glVertex2f(x-w,y+h);
  glEnd();
}

static void draw_border(float x, float y, float  w, float h, int lt) {

  glLineWidth(lt);

  glBegin(GL_LINE_LOOP);
  glVertex2f(x-w,y-h);
  glVertex2f(x+w,y-h);
  glVertex2f(x+w,y+h);
  glVertex2f(x-w,y+h);
  glEnd();
}


void vgui_tview::draw_tableau_icon(float x, float y) {

  draw_rect(x,y,icon_width,icon_height);

  glColor3f(1,1,1);
  glDisable(GL_LINE_STIPPLE);
  //glLineWidth(2);

  draw_border(x,y,icon_width, icon_height, 2);
}


void vgui_tview::draw_tview_icon(float x, float y) {


  glColor3f(1,0,0);
  draw_rect(x-icon_width/2.0, y-icon_height/2.0, icon_width/2.0, icon_height/2.0);

  glColor3f(0,1,0);
  draw_rect(x-icon_width/2.0, y+icon_height/2.0, icon_width/2.0, icon_height/2.0);

  glColor3f(0,0,1);
  draw_rect(x+icon_width/2.0, y+icon_height/2.0, icon_width/2.0, icon_height/2.0);

  glColor3f(1,1,0);
  draw_rect(x+icon_width/2.0, y-icon_height/2.0, icon_width/2.0, icon_height/2.0);

  glColor3f(1,1,1);
  glDisable(GL_LINE_STIPPLE);
  //glLineWidth(2);

  draw_border(x,y,icon_width, icon_height, 2);
}


void vgui_tview::draw_icons(vgui_tableau_sptr const& parent, float x, float y) {

  vcl_vector<vgui_tableau_sptr> children;
  parent->get_children(&children);

  if (children.size() > 0) {

    float ny = y + spacing*children.size()/10; // "spacing * (children.size()/10)" is float * (int/int), or float * int.
    ny = y + spacing*children.size(); // fsm@robots
    int num = children.size();

    float total_x = (num-1)*spacing;
    float start_x = x - (int) (total_x/2.0f); // why the cast?
    float offset_x = 0;

    //glLineStipple (1, 0x0101); // dotted
    //glLineStipple (1, 0x00FF); // dashed
    //glLineStipple (1, 0x1C47);  // dash/dot/dash
    glLineWidth(1);

    int ia = 0;
    for (vcl_vector<vgui_tableau_sptr>::iterator i = children.begin();
         i != children.end(); ++i, ++ia) {

      glDisable(GL_LINE_STIPPLE);
      glColor3f(1,1,1);

      if (parent->type_name() == "vgui_deck") {
        vgui_deck_sptr deck; deck.vertical_cast(parent);
        if (deck->current() != *i) {
          glLineStipple (1, 0x1C47);  // dash/dot/dash
          glEnable (GL_LINE_STIPPLE);
        }
      }
      else if (parent->type_name() == "vgui_composite") {
        vgui_composite_sptr comp; comp.vertical_cast(parent);

        if (!comp->is_active(ia)) {
          glLineStipple (1, 0x1C47);  // dash/dot/dash
          glEnable (GL_LINE_STIPPLE);
        }
      }
      else if (parent->type_name() == "vgui_listmanager2D") {
        vgui_listmanager2D_sptr lman; lman.vertical_cast(parent);
        if (!lman->is_active(ia)) {
          if (!lman->is_visible(ia))
            glLineStipple (1, 0x0101); // dotted
          else
            glLineStipple (1, 0x1C47);  // dash/dot/dash
          glEnable (GL_LINE_STIPPLE);
        }
      }

      glLineWidth(1);
      glBegin(GL_LINES);
      glVertex2f(x,y+icon_height);
      glVertex2f(start_x+offset_x,ny-(icon_height));
      glEnd();

      if (*i != 0)
        draw_icons(*i, start_x+offset_x, ny);
      offset_x+=spacing;
    }
  }


  if (parent->type_name() == "vgui_tview") {
    draw_tview_icon(x,y);
  }
  else {
    if (parent->type_name() == "vgui_deck")
      glColor3f(0,1,0);
    else if (parent->type_name() == "vgui_viewer2D")
      glColor3f(1,0,0);
    else if (parent->type_name() == "vgui_viewer3D")
      glColor3f(0, 0, 1);
    else if (parent->type_name() == "vgui_displaylist2D")
      glColor3f(0.5, 0, 0);
    else if (parent->type_name() == "vgui_displaylist3D")
      glColor3f(0, 0, 0.5);
    else if (parent->type_name() == "vgui_listmanager2D")
      glColor3f(1, 0, 0.5);
    else if (parent->type_name() == "vgui_composite")
      glColor3f(1, 0, 1);
    else if (parent->type_name() == "vgui_image_tableau")
      glColor3f(1, 0.5, 0);
    else if (parent->type_name() == "vgui_vrml")
      glColor3f(0, 1, 1);
    else if (parent->type_name() == "vgui_blackbox")
      glColor3f(0, 0, 0);
    else
      glColor3f(1,1,0);
    draw_tableau_icon(x,y);
  }

  if (parent == active_icon) {
    glColor3f(1,1,1);
    draw_border(x,y,icon_width,icon_height, 6);

    glColor3f(1,0,0);
    draw_border(x,y,icon_width,icon_height, 2);
  }
}


void vgui_tview::add_icons(vcl_vector<vgui_tview::icon>* icons, vgui_tableau_sptr const& parent,
                           float x, float y)
{
  vcl_vector<vgui_tableau_sptr> children;
  parent->get_children(&children);

  if (children.size() > 0) {

    float ny = y+spacing;
    int num = children.size();

    float total_x = (num-1)*spacing;
    float start_x = x - (int) (total_x/2.0);
    float offset_x = 0;


    for (vcl_vector<vgui_tableau_sptr>::iterator i = children.begin();
         i != children.end(); ++i) {

      add_icons(icons, *i, start_x+offset_x, ny);
      offset_x+=spacing;
    }
  }

  // add this icon
  icon this_icon;
  this_icon.tableau = parent;
  this_icon.x = x;
  this_icon.y = y;
  icons->push_back(this_icon);
}

vgui_tableau_sptr vgui_tview::find_closest_icon(vcl_vector<vgui_tview::icon> const& icons, float ix, float iy) {
  //cerr << "number of icons " << icons.size() << endl;

  float closest_dist /*quell SunPro warning*/= -1;
  vgui_tableau_sptr closest;

  for (vcl_vector<icon>::const_iterator i_iter = icons.begin();
       i_iter != icons.end(); ++i_iter) {
    icon i = *i_iter;
    float dist = hypot(i.x - ix, i.y - iy);
    if (!closest || dist < closest_dist) {
      closest_dist = dist;
      closest = i.tableau;
    }
  }

  return closest;
}


vcl_string strip_preceeding_numerals(const char* name) {
  vcl_string str(name);

  vcl_string::iterator s_iter = str.begin();
  for (; s_iter != str.end(); ++s_iter) {
    if (*s_iter < '0' || *s_iter > '9') {
      break;
    }
  }

  if (s_iter != str.end())
    str.erase(str.begin(), s_iter);

  return str;
}

bool vgui_tview::handle(const vgui_event& e) {

  if (debug) vcl_cerr << "vgui_tview::handle" << vcl_endl;

  GLfloat vp[4];
  glGetFloatv(GL_VIEWPORT, vp); // ok
  float width = vp[2];
  float height = vp[3];

  float startx = width/2;
  float starty = height/3;

  if (e.type == vgui_DRAW) {

    // explicitly clearing colour as we dont want colour of
    // any tableaux this might be embedded within
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw the child
    draw_icons(child, startx, starty);
    return true;
  }
  else if (e.type==vgui_MOTION) {
    vcl_vector<icon> icons;
    add_icons(&icons, child, startx, starty);

    vgui_projection_inspector pi;
    float ix, iy;
    pi.window_to_image_coordinates(e.wx, e.wy, ix, iy);

    closest_icon = find_closest_icon(icons, ix, iy);
  }

  else if (e.type==vgui_BUTTON_DOWN) {
    vcl_vector<icon> icons;
    add_icons(&icons, child, startx, starty);

    vgui_projection_inspector pi;
    float ix, iy;
    pi.window_to_image_coordinates(e.wx, e.wy, ix, iy);

    vgui_tableau_sptr t = find_closest_icon(icons, ix, iy);
    active_icon = t;

    //cerr << "icon is " << (void*) t << endl;
    if (t != 0) {
      vcl_cerr << "---------" << vcl_endl;
      //cerr << "| type : " << strip_preceeding_numerals(typeid(*t).name()) << endl;
      vcl_cerr << "| type_name   : " << t->type_name() << vcl_endl;
      vcl_cerr << "| file_name   : " << t->file_name() << vcl_endl;
      vcl_cerr << "| pretty_name : " << t->pretty_name() << vcl_endl;
      vcl_cerr << "---------" << vcl_endl << vcl_endl;;
    }

    post_redraw();
  }
  else if (e.type==vgui_BUTTON_UP) {
    active_icon = 0;
    post_redraw();
  }

  else {
    GLint oldbuff;
    glGetIntegerv(GL_DRAW_BUFFER, &oldbuff);
    glDrawBuffer(GL_NONE);
    bool retv = child->handle(e);
    glDrawBuffer((GLenum)oldbuff);
    return retv;
  }

  return false;
}


void vgui_tview::get_popup(const vgui_popup_params& /*params*/, vgui_menu &menu) {
  if (closest_icon) {
    menu.separator();
    vgui_popup_params np;
    np.recurse = false;
    closest_icon->get_popup(np, menu);
  }
}
