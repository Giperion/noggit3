// UICheckBox.cpp is part of Noggit3, licensed via GNU General Publiicense (version 3).
// Bernd Lörwald <bloerwald+noggit@googlemail.com>
// Stephan Biegel <project.modcraft@googlemail.com>

#include <noggit/UICheckBox.h>

#include <string>

#include <noggit/application.h> // arialn13
#include <noggit/UIText.h>
#include <noggit/UITexture.h>
#include <noggit/UIToggleGroup.h>

UICheckBox::UICheckBox( float xPos, float yPos, const std::string& pText )
: UIFrame( xPos, yPos, 30.0f, 30.0f )
, check( new UITexture( 0.0f, 0.0f, 32.0f, 32.0f, "Interface\\Buttons\\UI-CheckBox-Check.blp" ) )
, text( new UIText( 32.0f, 8.0f, pText, arialn13, eJustifyLeft ) )
, checked( false )
, id( 0 )
, clickFunc( NULL )
, mToggleGroup( NULL )
{
  addChild( new UITexture( 0.0f, 0.0f, 32.0f, 32.0f, "Interface\\Buttons\\UI-CheckBox-Up.blp" ) );
  check->hide();
  addChild( check );
  addChild( text );
}

UICheckBox::UICheckBox( float xPos, float yPos, const std::string& pText, void (*pClickFunc)(bool,int), int pClickFuncParameter )
: UIFrame( xPos, yPos, 30.0f, 30.0f )
, check( new UITexture( 0.0f, 0.0f, 32.0f, 32.0f, "Interface\\Buttons\\UI-CheckBox-Check.blp" ) )
, text( new UIText( 32.0f, 8.0f, pText, arialn13, eJustifyLeft ) )
, checked( false )
, id( pClickFuncParameter )
, clickFunc( pClickFunc )
, mToggleGroup( NULL )
{
  addChild( new UITexture( 0.0f, 0.0f, 32.0f, 32.0f, "Interface\\Buttons\\UI-CheckBox-Up.blp" ) );
  check->hide();
  addChild( check );
  addChild( text );
}

UICheckBox::UICheckBox( float xPos, float yPos, const std::string& pText, UIToggleGroup * pToggleGroup, int pToggleID )
: UIFrame( xPos, yPos, 30.0f, 30.0f )
, check( new UITexture( 0.0f, 0.0f, 32.0f, 32.0f, "Interface\\Buttons\\UI-CheckBox-Check.blp" ) )
, text( new UIText( 32.0f, 8.0f, pText, arialn13, eJustifyLeft ) )
, checked( false )
, id( 0 )
, clickFunc( NULL )
, mToggleGroup( pToggleGroup )
{
  addChild( new UITexture( 0.0f, 0.0f, 32.0f, 32.0f, "Interface\\Buttons\\UI-CheckBox-Up.blp" ) );
  check->hide();
  addChild( check );
  addChild( text );

  mToggleGroup->Add( this, pToggleID );
}

void UICheckBox::SetToggleGroup( UIToggleGroup * pToggleGroup, int pToggleID )
{
  mToggleGroup = pToggleGroup;
  if( mToggleGroup )
    mToggleGroup->Add( this, pToggleID );
  clickFunc = NULL;
}

void UICheckBox::setText( const std::string& pText )
{
  text->setText( pText );
}

void UICheckBox::setState( bool c )
{
  checked = c;
  check->hidden( !checked );
}

bool UICheckBox::getState()
{
  return checked;
}

UIFrame* UICheckBox::processLeftClick( float /*mx*/, float /*my*/ )
{
  checked = !checked;
  check->hidden( !checked );

  if( mToggleGroup )
    mToggleGroup->Activate( this );
  else if( clickFunc )
    clickFunc( checked, id );

  return this;
}

void UICheckBox::setClickFunc( void (*f)(bool,int), int i )
{
  id = i;
  clickFunc = f;
  mToggleGroup = NULL;
}
