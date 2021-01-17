// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#include <QtCore/QString>
#include <QtGui/QFontDatabase>
#include <QtGui/QIconEngine>
#include <QtGui/QPainter>
#include <QtWidgets/QMessageBox>

#include <noggit/Log.h>
#include <noggit/ui/font_noggit.hpp>

namespace noggit
{
	namespace ui
	{
		font_noggit_icon_engine::font_noggit_icon_engine(const QString& text)
			: font_awesome_icon_engine(text), _text(text)
		{
		}

		font_noggit_icon_engine* font_noggit_icon_engine::clone() const
		{
			return new font_noggit_icon_engine(_text);
		}

		void font_noggit_icon_engine::paint(QPainter* painter,
											QRect const& rect, QIcon::Mode mode,
											QIcon::State state)
		{
			painter->save();
			{
				painter->setPen((state == QIcon::On || mode == QIcon::Active)
									? QColor(0, 0, 0)
									: QColor(0, 0, 0));

				if (!_fonts.count(rect.height()))
				{
					auto id(QFontDatabase::addApplicationFont(
						":/fonts/noggit_font.ttf"));

					if (id == -1)
					{
						throw std::runtime_error("Unable to load Noggit font.");
					}

					QFont font(
						QFontDatabase::applicationFontFamilies(id).at(0));
					font.setPixelSize(rect.height());

					_fonts[rect.height()] = font;
				}

				painter->setFont(_fonts.at(rect.height()));

				painter->drawText(
					rect, _text,
					QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
			}
			painter->restore();
		}

		std::map<int, QFont> font_noggit_icon_engine::_fonts = {};

		font_noggit_icon::font_noggit_icon(font_noggit::icons const& icon)
			: QIcon(new font_noggit_icon_engine(QString(QChar(icon))))
		{
		}

	}  // namespace ui
}  // namespace noggit
