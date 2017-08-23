// @file:     application.cc
// @author:   Jake
// @created:  2016.10.31
// @editted:  2017.05.09  - Jake
// @license:  GNU LGPL v3
//
// @desc:     Definitions of the Application functions



// Qt includes
#include <QtSvg>

// gui includes
#include "application.h"
#include "src/settings/settings.h"

#include <iostream>


// init the DialogPanel to NULL until build in constructor
gui::DialogPanel *gui::ApplicationGUI::dialog_pan = 0;


// constructor
gui::ApplicationGUI::ApplicationGUI(QWidget *parent)
  : QMainWindow(parent)
{
  // save start time for instance recognition
  start_time = QDateTime::currentDateTime();

  // initialise GUI
  initGUI();

  // load settings, resize mainwindow
  loadSettings();
}

// destructor
gui::ApplicationGUI::~ApplicationGUI()
{
  // save current settings
  saveSettings();

  // free memory, parent delete child Widgets so Graphical Items are already
  // handled. Still need to free Settings

  delete settings::AppSettings::instance();
  delete settings::GUISettings::instance();
  delete settings::LatticeSettings::instance();
  delete prim::Emitter::instance();
}



// GUI INITIALISATION

void gui::ApplicationGUI::initGUI()
{
  // initialise bars
  initMenuBar();
  initTopBar();
  initSideBar();

  // initialise mainwindow panels
  dialog_pan = new gui::DialogPanel(this);
  design_pan = new gui::DesignPanel(this);
  input_field = new gui::InputField(this);
  info_pan = new gui::InfoPanel(this);

  // layout management
  QWidget *main_widget = new QWidget(this); // main widget for mainwindow
  QVBoxLayout *vbl = new QVBoxLayout();     // main layout, vertical
  QHBoxLayout *hbl = new QHBoxLayout();     // lower layout, horizontal
  QVBoxLayout *vbl_l = new QVBoxLayout();   // dialog/input layout, vertical

  vbl_l->addWidget(dialog_pan, 1);
  vbl_l->addWidget(input_field, 0);

  hbl->addLayout(vbl_l, 1);
  hbl->addWidget(info_pan, 1);

  vbl->addWidget(design_pan, 2);
  vbl->addLayout(hbl, 1);

  // set mainwindow layout
  main_widget->setLayout(vbl);
  setCentralWidget(main_widget);

  // additional actions
  initActions();

  // prepare initial GUI state
  initState();
}


void gui::ApplicationGUI::initMenuBar()
{
  // initialise menus
  QMenu *file = menuBar()->addMenu(tr("&File"));
  // QMenu *edit = menuBar()->addMenu(tr("&Edit"));
  // QMenu *view = menuBar()->addMenu(tr("&View"));
  menuBar()->addMenu(tr("&Edit"));
  menuBar()->addMenu(tr("&View"));
  QMenu *tools = menuBar()->addMenu(tr("&Tools"));

  // file menu actions
  QAction *new_file = new QAction(tr("&New"), this);
  QAction *quit = new QAction(tr("&Quit"), this);
  QAction *save = new QAction(tr("&Save"), this);
  QAction *save_as = new QAction(tr("Save As..."), this);
  QAction *open_save = new QAction(tr("&Open..."), this);
  quit->setShortcut(tr("CTRL+Q"));
  save->setShortcut(tr("CTRL+S"));
  save_as->setShortcut(tr("CTRL+SHIFT+S"));
  open_save->setShortcut(tr("CTRL+O"));
  file->addAction(new_file);
  file->addAction(save);
  file->addAction(save_as);
  file->addAction(open_save);
  file->addAction(quit);

  QAction *change_lattice = new QAction(tr("Change Lattice..."), this);
  QAction *select_color = new QAction(tr("Select Color..."), this);
  QAction *screenshot = new QAction(tr("Full Screenshot..."), this);
  QAction *design_screenshot = new QAction(tr("Design Screenshot..."), this);

  tools->addAction(change_lattice);
  tools->addAction(select_color);
  tools->addAction(screenshot);
  tools->addAction(design_screenshot);

  connect(new_file, &QAction::triggered, this, &gui::ApplicationGUI::newFile);
  //connect(quit, &QAction::triggered, qApp, QApplication::quit);
  connect(quit, &QAction::triggered, this, &gui::ApplicationGUI::closeFile);
  connect(save, &QAction::triggered, this, &gui::ApplicationGUI::saveDefault);
  connect(save_as, &QAction::triggered, this, &gui::ApplicationGUI::saveNew);
  connect(open_save, &QAction::triggered, this, &gui::ApplicationGUI::openFromFile);
  connect(change_lattice, &QAction::triggered, this, &gui::ApplicationGUI::changeLattice);
  connect(select_color, &QAction::triggered, this, &gui::ApplicationGUI::selectColor);
  connect(screenshot, &QAction::triggered, this, &gui::ApplicationGUI::screenshot);
  connect(design_screenshot, &QAction::triggered, this, &gui::ApplicationGUI::designScreenshot);

}


void gui::ApplicationGUI::initTopBar()
{
  settings::GUISettings *gui_settings = settings::GUISettings::instance();

  top_bar = new QToolBar(tr("Top Bar"));

  // location behaviour
  top_bar->setFloatable(false);
  top_bar->setMovable(false);

  // size policy
  qreal ico_scale = gui_settings->get<qreal>("SBAR/mw");
  ico_scale *= gui_settings->get<qreal>("SBAR/ico");

  top_bar->setMinimumHeight(gui_settings->get<int>("TBAR/mh"));
  top_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  top_bar->setIconSize(QSize(ico_scale, ico_scale));

  action_run_sim = top_bar->addAction(QIcon(":/ico/runsim.svg"), tr("Run Simulation..."));

  connect(action_run_sim, &QAction::triggered, this, &gui::ApplicationGUI::runSimulation);
  addToolBar(Qt::TopToolBarArea, top_bar);
}


void gui::ApplicationGUI::initSideBar()
{
  settings::GUISettings *gui_settings = settings::GUISettings::instance();

  // recall or initialise side bar location
  Qt::ToolBarArea area;
  if(gui_settings->contains("SBAR/loc"))
    area = static_cast<Qt::ToolBarArea>(gui_settings->get<int>("SBAR/loc"));
  else
    area = Qt::LeftToolBarArea;

  side_bar = new QToolBar(tr("Side Bar"));

  // location behaviour
  side_bar->setAllowedAreas(Qt::LeftToolBarArea|Qt::RightToolBarArea);
  side_bar->setFloatable(false);

  // size policy
  qreal ico_scale = gui_settings->get<qreal>("SBAR/mw");
  ico_scale *= gui_settings->get<qreal>("SBAR/ico");

  side_bar->setMinimumWidth(gui_settings->get<int>("SBAR/mw"));
  side_bar->setIconSize(QSize(ico_scale, ico_scale));

  // actions
  QActionGroup *action_group = new QActionGroup(side_bar);

  action_select_tool = side_bar->addAction(QIcon(":/ico/select.svg"), tr("Select tool"));
  action_drag_tool = side_bar->addAction(QIcon(":/ico/drag.svg"), tr("Drag tool"));
  action_dbgen_tool = side_bar->addAction(QIcon(":/ico/dbgen.svg"), tr("DB tool"));

  action_group->addAction(action_select_tool);
  action_group->addAction(action_drag_tool);
  action_group->addAction(action_dbgen_tool);

  action_select_tool->setCheckable(true);
  action_drag_tool->setCheckable(true);
  action_dbgen_tool->setCheckable(true);

  action_select_tool->setChecked(true);

  connect(action_select_tool, &QAction::triggered, this, &gui::ApplicationGUI::setToolSelect);
  connect(action_drag_tool, &QAction::triggered, this, &gui::ApplicationGUI::setToolDrag);
  connect(action_dbgen_tool, &QAction::triggered, this, &gui::ApplicationGUI::setToolDBGen);

  addToolBar(area, side_bar);
}


void gui::ApplicationGUI::initActions()
{
  // input field
  connect(input_field, &gui::InputField::returnPressed,
            this, &gui::ApplicationGUI::parseInputField);

  // set tool
  connect(design_pan, &gui::DesignPanel::sig_toolChange,
            this, &gui::ApplicationGUI::setTool);
}


void gui::ApplicationGUI::initState()
{
  settings::AppSettings *app_settings = settings::AppSettings::instance();

  setTool(gui::DesignPanel::SelectTool);
  working_path.clear();
  autosave_timer.start(1000*app_settings->get<int>("save/autosaveinterval"));
}



// SETTINGS

void gui::ApplicationGUI::loadSettings()
{
  qDebug() << tr("Loading settings");
  settings::GUISettings *gui_settings = settings::GUISettings::instance();

  resize(gui_settings->get<QSize>("MWIN/size"));

  // autosave related settings
  settings::AppSettings *app_settings = settings::AppSettings::instance();
  autosave_num = app_settings->get<int>("save/autosavenum");
  autosave_root.setPath(app_settings->get<QString>("save/autosaveroot"));

  // autosave directory for current instance
  qint64 tag = QCoreApplication::applicationPid();
  //QString tag = start_time.toString("yyMMdd-HHmmss")
  autosave_dir.setPath(autosave_root.filePath(tr("instance-%1").arg(tag)));
  autosave_dir.setNameFilters(QStringList() << "*.*");
  autosave_dir.setFilter(QDir::Files);

  // create the autosave directory
  if(!autosave_dir.exists()){
    if(autosave_dir.mkpath("."))
      qDebug() << tr("Successfully created autosave directory");
    else
      qCritical() << tr("Failed to create autosave direcrory");
  }

  // auto save signal
  connect(&autosave_timer, &QTimer::timeout, this, &gui::ApplicationGUI::autoSave);

  // reset state
  initState();
}


void gui::ApplicationGUI::saveSettings()
{
  qDebug() << tr("Saving settings");
  settings::GUISettings *gui_settings = settings::GUISettings::instance();

  gui_settings->setValue("SBAR/loc", (int) toolBarArea(side_bar));

  // remove autosave during peaceful termination
  for(const QString &dirFile : autosave_dir.entryList())
    autosave_dir.remove(dirFile); // remove autosave files

  qDebug() << tr("Test: %1").arg(autosave_dir.absolutePath());

  if(autosave_dir.removeRecursively())
    qDebug() << tr("Removed autosave directory: %1").arg(autosave_dir.path());
  else
    qDebug() << tr("Failed to remove autosave directory: %1").arg(autosave_dir.path());
}




// PUBLIC SLOTS

void gui::ApplicationGUI::setTool(gui::DesignPanel::ToolType tool)
{
  switch(tool){
    case gui::DesignPanel::SelectTool:
      action_select_tool->setChecked(true);
      setToolSelect();
      break;
    case gui::DesignPanel::DragTool:
      action_drag_tool->setChecked(true);
      setToolDrag();
      break;
    case gui::DesignPanel::DBGenTool:
      action_dbgen_tool->setChecked(true);
      setToolDBGen();
      break;
    default:
      break;
  }
}

void gui::ApplicationGUI::setToolSelect()
{
  qDebug() << tr("selecting select tool");
  design_pan->setTool(gui::DesignPanel::SelectTool);
}

void gui::ApplicationGUI::setToolDrag()
{
  qDebug() << tr("selecting drag tool");
  design_pan->setTool(gui::DesignPanel::DragTool);
}

void gui::ApplicationGUI::setToolDBGen()
{
  qDebug() << tr("selecting dbgen tool");
  design_pan->setTool(gui::DesignPanel::DBGenTool);
}

void gui::ApplicationGUI::changeLattice()
{
  settings::AppSettings *app_settings = settings::AppSettings::instance();

  QString dir = app_settings->get<QString>("dir/lattice");
  QString fname = QFileDialog::getOpenFileName(
    this, tr("Select lattice file"), dir, tr("INI (*.ini)"));

  design_pan->buildLattice(fname);
}

void gui::ApplicationGUI::parseInputField()
{
  // get input from input_field, remove leading/trailing whitespace
  QString input = input_field->pop();

  // if input string is not empty, do something
  if(!input.isEmpty()){
    // for now, just echo input to stdout
    qDebug() << input;
  }
}

void gui::ApplicationGUI::runSimulation()
{
  qDebug() << tr("Simulation functionality currently disabled...");
}





// SANDBOX

void gui::ApplicationGUI::selectColor()
{
  QColorDialog::getColor(Qt::white, this,
    tr("Select a color"), QColorDialog::ShowAlphaChannel);
}


void gui::ApplicationGUI::screenshot()
{
  // get save path
  QString fname = QFileDialog::getSaveFileName(this, tr("Save File"), img_dir.path(),
                      tr("SVG files (*.svg)"));

  if(fname.isEmpty())
    return;
  img_dir = QDir(fname);

  gui::ApplicationGUI *widget = this;
  QRect rect = widget->rect();
  initState();

  QSvgGenerator gen;
  gen.setFileName(fname);
  gen.setSize(rect.size());
  gen.setViewBox(rect);

  QPainter painter;
  painter.begin(&gen);
  widget->render(&painter);

  // render menus
  QRect r; QString s;
  for(QAction *act : menuBar()->actions()){
    r = menuBar()->actionGeometry(act);
    s = act->text();
    while(s.startsWith("&"))
      s.remove(0,1);
    painter.drawText(r, Qt::AlignCenter, s);
  }

  painter.end();
}


void gui::ApplicationGUI::designScreenshot()
{
  // get save path
  QString fname = QFileDialog::getSaveFileName(this, tr("Save File"), img_dir.path(),
                      tr("SVG files (*.svg)"));

  if(fname.isEmpty())
    return;
  img_dir = QDir(fname);

  gui::DesignPanel *widget = this->design_pan;
  QRect rect = widget->rect();
  rect.setHeight(rect.height() - widget->horizontalScrollBar()->height());
  rect.setWidth(rect.width() - widget->verticalScrollBar()->width());

  QSvgGenerator gen;
  gen.setFileName(fname);
  gen.setSize(rect.size());
  gen.setViewBox(rect);

  QPainter painter;
  painter.begin(&gen);
  widget->render(&painter);
  painter.end();
}


// FILE HANDLING


// unsaved changes prompt, returns whether to proceed with operation or not
bool gui::ApplicationGUI::resolveUnsavedChanges()
{
  QMessageBox msg_box;
  msg_box.setText("The document contains unsaved changes.");
  msg_box.setInformativeText("Would you like to save?");
  msg_box.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
  msg_box.setDefaultButton(QMessageBox::Save);

  // proceed only if either 'Save' or 'Discard'
  int usr_sel = msg_box.exec();
  return (usr_sel==QMessageBox::Save) ? saveToFile() : usr_sel==QMessageBox::Discard;
}


// make new file
void gui::ApplicationGUI::newFile()
{
  // prompt user to resolve unsaved changes if program has been modified
  if(design_pan->stateChanged())
    if(!resolveUnsavedChanges())
      return;

  // reset widgets and reinitialize GUI state
  design_pan->resetDesignPanel();
  initState();
}


// save/load:
bool gui::ApplicationGUI::saveToFile(gui::ApplicationGUI::SaveFlag flag, const QString &path)
{
  QString write_path;

  // determine target file
  if(!path.isEmpty())
    write_path = path;
  else if(working_path.isEmpty() || flag==SaveAs){
    write_path = QFileDialog::getSaveFileName(this, tr("Save File"),
                  save_dir.filePath("cooldbdesign.xml"), tr("XML files (*.xml)"));
    if(write_path.isEmpty())
      return false;
  }
  else
    write_path = working_path;

  // add .xml extension if there isn't
  if(!write_path.endsWith(".xml", Qt::CaseInsensitive))
    write_path.append(".xml");

  // set file name of [whatevername].writing while writing to prevent loss of previous save if this save fails
  QFile file(write_path+".writing");

  qDebug() << write_path;

  if(!file.open(QIODevice::WriteOnly)){
    qDebug() << tr("Save: Error when opening file to save: %1").arg(file.errorString());
    return false;
  }

  // write to XML stream
  QXmlStreamWriter stream(&file);
  qDebug() << tr("Save: Beginning write to %1").arg(file.fileName());
  stream.setAutoFormatting(true);
  stream.writeStartDocument();

  // call the save functions for each relevant class
  stream.writeStartElement("dbdesigner");
  design_pan->saveToFile(&stream);
  stream.writeEndElement();

  // close the file
  file.close();

  // delete the existing file and rename the new one to it
  QFile::remove(write_path);
  file.rename(write_path);

  qDebug() << tr("Save: Write completed for %1").arg(file.fileName());

  // update working path if needed
  if(flag != AutoSave){
    save_dir.setPath(write_path);
    working_path = write_path;
  }

  return true;
}


void gui::ApplicationGUI::saveDefault()
{
  // default manual save without forcing file chooser
  if(saveToFile(Save))
    design_pan->stateSet();
}


void gui::ApplicationGUI::saveNew()
{
  // save to new file path with file chooser
  if(saveToFile(SaveAs))
    design_pan->stateSet();
}


void gui::ApplicationGUI::autoSave()
{
  // no check for changes in state... unneccesary complexity

  qDebug() << tr("Autosave: %1").arg(autosave_dir.absolutePath());
  if(!autosave_dir.exists()){
    qCritical() << tr("Autosave: unable to create tmp instance directory at %1").arg(autosave_dir.path());
    return;
  }

  autosave_ind = (autosave_ind+1) % autosave_num;
  QString autosave_path = autosave_dir.filePath(tr("autosave-%1.xml").arg(autosave_ind));
  saveToFile(AutoSave, autosave_path);

  qDebug() << tr("Autosave complete");
}


void gui::ApplicationGUI::openFromFile()
{
  // prompt user to resolve unsaved changes if program has been modified
  if(design_pan->stateChanged())
    if(!resolveUnsavedChanges())
      return;

  // file dialog
  QString prompt_path = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("XML files (*.xml)"));

  if(prompt_path.isEmpty())
    return;

  working_path = prompt_path;
  QFile file(working_path);

  if(!file.open(QFile::ReadOnly | QFile::Text)){
    qDebug() << tr("Error when opening file to read: %1").arg(file.errorString());
    return;
  }

  // read from XML stream
  QXmlStreamReader stream(&file);
  qDebug() << tr("Beginning load from %1").arg(file.fileName());
  design_pan->loadFromFile(&stream);
  qDebug() << tr("Load complete");
  file.close();
}


void gui::ApplicationGUI::closeFile()
{
  // prompt user to resolve unsaved changes if program has been modified
  if(design_pan->stateChanged())
    if(!resolveUnsavedChanges())
      return;

  QApplication::quit();
}
