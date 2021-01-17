// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#include <noggit/AsyncLoader.h>
#include <noggit/DBC.h>
#include <noggit/Log.h>
#include <noggit/MPQ.h>
#include <noggit/MapView.h>
#include <noggit/Model.h>
#include <noggit/ModelManager.h>	// ModelManager::report()
#include <noggit/TextureManager.h>  // TextureManager::report()
#include <noggit/WMO.h>				// WMOManager::report()
#include <noggit/errorHandling.h>
#include <noggit/liquid_layer.hpp>
#include <noggit/ui/main_window.hpp>
#include <opengl/context.hpp>
#include <util/exception_to_string.hpp>

#include <filesystem>

#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtGui/QOffscreenSurface>
#include <QtOpenGL/QGLFormat>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "revision.h"

class Noggit
{
   public:
	Noggit(int argc, char* argv[]);

   private:
	void initPath(char* argv[]);
	void loadMPQs();

	std::unique_ptr<noggit::ui::main_window> main_window;

	std::filesystem::path wowpath;

	bool fullscreen;
	bool doAntiAliasing;
};

void Noggit::initPath(char* argv[])
{
	try
	{
		std::filesystem::path startupPath(argv[0]);
		startupPath.remove_filename();

		if (startupPath.is_relative())
		{
			std::filesystem::current_path(std::filesystem::current_path() /
										  startupPath);
		}
		else
		{
			std::filesystem::current_path(startupPath);
		}
	}
	catch (const std::filesystem::filesystem_error& ex)
	{
		LogError << ex.what() << endl;
	}
}

void Noggit::loadMPQs()
{
	std::vector<std::string> archiveNames;
	archiveNames.emplace_back("base.MPQ");
	archiveNames.emplace_back("dbc.MPQ");
	archiveNames.emplace_back("fonts.MPQ");
	archiveNames.emplace_back("interface.MPQ");
	archiveNames.emplace_back("misc.MPQ");
	archiveNames.emplace_back("model.MPQ");
	archiveNames.emplace_back("terrain.MPQ");
	archiveNames.emplace_back("texture.MPQ");
	archiveNames.emplace_back("wmo.MPQ");
	archiveNames.emplace_back("patch.MPQ");
	archiveNames.emplace_back("patch-2.MPQ");
	archiveNames.emplace_back("patch-T.MPQ");
	archiveNames.emplace_back("patch-U.MPQ");

	//! \todo  This may be done faster. Maybe.
	for (size_t i(0); i < archiveNames.size(); ++i)
	{
		std::string path((wowpath / "Data" / archiveNames[i]).string());
		MPQArchive::loadMPQ(&AsyncLoader::instance(), path, true);
	}
}

namespace
{
	bool is_valid_game_path(const QDir& path)
	{
		if (!path.exists())
		{
			LogError << "Path \"" << qPrintable(path.absolutePath())
					 << "\" does not exist." << endl;
			return false;
		}

		if (path.exists(QStringLiteral("Data/base.mpq")))
		{
			return true;
		}

		LogError << "Path \"" << qPrintable(path.absolutePath())
				 << "\" is not valid." << endl;

		return false;
	}
}  // namespace

Noggit::Noggit(int argc, char* argv[]) : fullscreen(false), doAntiAliasing(true)
{
	InitLogging();
	assert(argc >= 1);
	(void)argc;
	initPath(argv);

	Log << "Noggit Studio - " << STRPRODUCTVER << endl;

	QSettings settings;
	doAntiAliasing = settings.value("antialiasing", false).toBool();
	fullscreen = settings.value("fullscreen", false).toBool();

	srand(::time(nullptr));
	QDir path(settings.value("project/game_path").toString());

	while (!is_valid_game_path(path))
	{
		QDir new_path(QFileDialog::getExistingDirectory(
			nullptr, "Open WoW Directory", "/", QFileDialog::ShowDirsOnly));
		if (new_path.absolutePath() == "")
		{
			LogError << "Could not auto-detect game path "
					 << "and user canceled the dialog." << endl;
			throw std::runtime_error("no folder chosen");
		}
		std::swap(new_path, path);
	}

	wowpath = path.absolutePath().toStdString();

	Log << "Game path: " << wowpath << endl;

	std::string project_path =
		settings.value("project/path", path.absolutePath())
			.toString()
			.toStdString();
	settings.setValue("project/path", QString::fromStdString(project_path));

	Log << "Project path: " << project_path << endl;

	settings.setValue("project/game_path", path.absolutePath());
	settings.setValue("project/path", QString::fromStdString(project_path));

	loadMPQs();  // listfiles are not available straight away! They are async!
				 // Do not rely on anything at this point!
	OpenDBs();

	if (!QGLFormat::hasOpenGL())
	{
		throw std::runtime_error(
			"Your system does not support OpenGL. Sorry, this application "
			"can't run without it.");
	}

	QSurfaceFormat format;

	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setVersion(3, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);

	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	format.setSwapInterval(settings.value("vsync", 0).toInt());

	if (doAntiAliasing)
	{
		format.setSamples(4);
	}

	QSurfaceFormat::setDefaultFormat(format);

	QOpenGLContext context;
	context.create();
	QOffscreenSurface surface;
	surface.create();
	context.makeCurrent(&surface);

	opengl::context::scoped_setter const _(::gl, &context);

	LogDebug << "GL: Version: " << QString::fromLatin1((char*)gl.getString(GL_VERSION)) << endl;
	LogDebug << "GL: Vendor: " << QString::fromLatin1((char*)gl.getString(GL_VENDOR))
			 << endl;
	LogDebug << "GL: Renderer: " << QString::fromLatin1 ((char*)gl.getString(GL_RENDERER))
			 << endl;

	main_window = std::make_unique<noggit::ui::main_window>();
	if (fullscreen)
	{
		main_window->showFullScreen();
	}
	else
	{
		main_window->showMaximized();
	}
}

namespace
{
	void noggit_terminate_handler()
	{
		std::string const reason{
			util::exception_to_string(std::current_exception())};

		if (qApp)
		{
			QMessageBox::critical(nullptr, "std::terminate",
								  QString::fromStdString(reason),
								  QMessageBox::Close, QMessageBox::Close);
		}

		LogError << "std::terminate: " << reason << endl;
	}

	struct application_with_exception_printer_on_notify : QApplication
	{
		using QApplication::QApplication;

		virtual bool notify(QObject* object, QEvent* event) override
		{
			try
			{
				return QApplication::notify(object, event);
			}
			catch (...)
			{
				std::terminate();
			}
		}
	};
}  // namespace

int main(int argc, char* argv[])
{
	noggit::RegisterErrorHandlers();
	std::set_terminate(noggit_terminate_handler);

	QApplication qapp(argc, argv);
	qapp.setApplicationName("Noggit");
	qapp.setOrganizationName("Noggit");

	Noggit app(argc, argv);

	return qapp.exec();
}
