// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/TexturingGUI.h>

#include <algorithm>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <noggit/DBC.h>
#include <noggit/Misc.h>
#include <noggit/MPQ.h>
#include <noggit/Project.h>
#include <noggit/application.h> // app.getArial14(), app.getapp.getArialn13()()
#include <noggit/TextureManager.h> // TextureManager, Texture

#include <unordered_set>

#include <QtCore/QSortFilterProxyModel>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QListView>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QVBoxLayout>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/range/iterator_range.hpp>

namespace noggit
{
  namespace ui
  {
    tileset_chooser::tileset_chooser (QWidget* parent)
      : widget (parent)
    {
      while (!MPQArchive::allFinishedLoading())
      {
        MPQArchive::allFinishLoading();
      }

      std::vector<std::string> tilesets;
      std::unordered_set<std::string> tilesets_with_specular_variant;

      for (auto const& entry : gListfile)
      {
        if ( entry.find ("tileset") != std::string::npos
          && entry.find (".blp") != std::string::npos
           )
        {
          auto suffix_pos (entry.find ("_s.blp"));
          if (suffix_pos == std::string::npos)
          {
            tilesets.emplace_back (entry);
          }
          else
          {
            std::string specular (entry);
            specular.erase (suffix_pos, strlen ("_s"));
            tilesets_with_specular_variant.emplace (specular);
          }
        }
      }

      {
        auto const prefix
          (boost::filesystem::path (Project::getInstance()->getPath()));
        auto const prefix_size (prefix.string().length());

        if (boost::filesystem::exists (prefix))
        {
          for ( auto const& entry_abs
              : boost::make_iterator_range
                  (boost::filesystem::recursive_directory_iterator (prefix), {})
              )
          {
            auto entry ( noggit::mpq::normalized_filename
                          (entry_abs.path().string().substr (prefix_size))
                       );

            if ( entry.find ("tileset") != std::string::npos
              && entry.find (".blp") != std::string::npos
               )
            {
              auto suffix_pos (entry.find ("_s.blp"));
              if (suffix_pos == std::string::npos)
              {
                tilesets.emplace_back (entry);
              }
              else
              {
                std::string specular (entry);
                specular.erase (suffix_pos, strlen ("_s"));
                tilesets_with_specular_variant.emplace (specular);
              }
            }
          }
        }
      }


      auto model (new QStandardItemModel);
      constexpr int const has_specular_role = Qt::UserRole;

      for (auto const& texture : tilesets)
      {
        //! \todo render lazily
        auto item ( new QStandardItem
                      ( noggit::render_blp_to_pixmap (texture, 256, 256)
                      , QString::fromStdString (texture).remove ("tileset/")
                      )
                  );
        item->setData ( tilesets_with_specular_variant.count (texture) ? "true" : "false"
                      , has_specular_role
                      );
        model->appendRow (item);
      }

      auto specular_filter (new QSortFilterProxyModel);
      specular_filter->setSourceModel (model);
      specular_filter->setFilterRole (has_specular_role);

      auto search_filter (new QSortFilterProxyModel);
      search_filter->setSourceModel (specular_filter);
      search_filter->sort (0, Qt::AscendingOrder);


      auto filter (new QComboBox);
      filter->setEditable (true);
      filter->addItems ( { "", "base", "brick", "brush", "bush", "cobblestone"
                         , "crack", "creep", "crop", "crystal", "dark", "dead"
                         , "dirt", "fern", "floor", "flower", "footprints"
                         , "grass", "ice", "ivy", "lava", "leaf", "light"
                         , "mineral", "moss", "mud", "needle", "pebbl", "road"
                         , "rock", "root", "rubble", "sand", "shore", "slime"
                         , "smooth", "snow", "water", "waves", "web", "weed"
                         }
                       );
      connect ( filter, &QComboBox::currentTextChanged
              , [=] (QString text)
                {
                  search_filter->setFilterRegExp (text);
                }
              );


      auto only_specular (new QCheckBox ("only with specular texture variant"));
      connect ( only_specular, &QCheckBox::toggled
              , [=] (bool on)
                {
                  specular_filter->setFilterRegExp (on ? "true" : "");
                }
              );
      only_specular->setChecked (true);



      auto list (new QListView);
      list->setEditTriggers (QAbstractItemView::NoEditTriggers);
      list->setViewMode (QListView::IconMode);
      list->setMovement (QListView::Static);
      list->setResizeMode (QListView::Adjust);
      list->setUniformItemSizes (true);
      list->setIconSize ({128, 128});
      list->setWrapping (true);
      list->setModel (search_filter);

      connect ( list, &QAbstractItemView::doubleClicked
              , [=] (QModelIndex const& index)
                {
                  emit selected
                    ("tileset/" + index.data().toString().toStdString());
                }
              );

      auto size_slider (new QSlider (Qt::Horizontal));
      size_slider->setRange (64, 256);
      size_slider->setValue (128);
      connect ( size_slider, &QSlider::valueChanged
              , [=] (int size)
                {
                  list->setIconSize ({size, size});
                }
              );


      auto layout (new QVBoxLayout (this));
      auto top_bar (new QHBoxLayout);
      layout->addLayout (top_bar);
      top_bar->addWidget (size_slider);
      top_bar->addStretch();
      top_bar->addWidget (only_specular);
      top_bar->addWidget (filter);
      layout->addWidget (list);
    }
  }
}

boost::optional<scoped_blp_texture_reference> UITexturingGUI::selectedTexture = boost::none;

boost::optional<scoped_blp_texture_reference> UITexturingGUI::getSelectedTexture()
{
  return UITexturingGUI::selectedTexture;
}

void UITexturingGUI::setSelectedTexture (scoped_blp_texture_reference t)
{
  UITexturingGUI::selectedTexture = t;
}
