// UIListView.h is part of Noggit3, licensed via GNU General Publiicense (version 3).
// Bernd Lörwald <bloerwald+noggit@googlemail.com>
// Stephan Biegel <project.modcraft@googlemail.com>

#ifndef __UILISTVIEW_H
#define __UILISTVIEW_H

#include <noggit/UIFrame.h>
#include <noggit/UIScrollBar.h>

class UIListView : public UIFrame
{
public:
  typedef UIListView* Ptr;

private:
  int elements_height;
  int elements_start;
  int elements_rows;
  UIScrollBar::Ptr scrollbar;

public:
  UIListView( float xPos, float yPos, float w, float h, int elementHeight );
  void addElement( UIFrame* element );
  void setElementsHeight( int h );
  int getElementsCount();
  void recalcElements( unsigned int value );
  void clear();
};

#endif
