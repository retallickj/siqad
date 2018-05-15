// @file:     layer_manager.cc
// @author:   Samuel
// @created:  2017.12.22
// @editted:  2017.12.22 - Samuel
// @license:  GNU LGPL v3
//
// @desc:     widget for configuring

#include "layer_manager.h"

namespace gui {

// constructor
LayerManager::LayerManager(QWidget *parent)
  : QWidget(parent, Qt::Dialog)
{
  initLayerManager();
  initDockWidget();
}

// destructor
LayerManager::~LayerManager()
{
  clearLayerTable();

  // delete the layers
  removeAllLayers();
}

void LayerManager::addLayer(const QString &name, const prim::Layer::LayerType cnt_type, const float zoffset, const float zheight)
{
  // check if name already taken
  bool taken = false;
  for(prim::Layer *layer : layers) {
    if(layer->getName() == name){
      taken = true;
      break;
    }
  }

  if(taken){
    qWarning() << tr("A layer already exists with the name : %1").arg(name);
    return;
  }

  // layer is added to the end of layers stack, so ID = layers.size() before it was added
  prim::Layer *layer = new prim::Layer(name, cnt_type, zoffset, zheight, layers.size());
  layers.append(layer);
}

void LayerManager::removeLayer(const QString &name)
{
  bool removed = false;
  for(int i=0; i<layers.count(); i++){
    if(layers.at(i)->getName() == name){
      removeLayer(layers.at(i));
      removed=true;
      break;
    }
  }

  if(!removed)
    qWarning() << tr("Requested layer removal of %1 failed").arg(name);

}

void LayerManager::removeLayer(int n)
{
  if(n<0 ||  n>= layers.count()) {
    qWarning() << tr("Layer index out of bounds...");
  }else{
    prim::Layer *layer = layers.at(n);
    removeLayer(layer);
  }
}

void LayerManager::removeLayer(prim::Layer *layer)
{
  if(!layers.contains(layer)) {
    qFatal("Cannot remove layer, layer pointer doesn't exist");
  }else{
    int n = indexOf(layer);
    delete layer;
    layers.removeAt(n);

    // update layer_id for subsequent layers in the stack and their contained items
    for(int i=n; i<layers.count(); i++)
      layers.at(i)->setLayerIndex(i);

    // if top_layer was removed, default to surface if available else NULL
    if(active_layer==layer)
      active_layer = layers.count() > 1 ? layers.at(1) : 0;
  }
}

void LayerManager::removeAllLayers()
{
  while (!layers.isEmpty())
    removeLayer(0);
}

prim::Layer* LayerManager::getLayer(const QString &name) const
{
  for(prim::Layer *layer : layers)
    if(layer->getName() == name)
      return layer;

  // no layer had a matching name, return 0
  qWarning() << tr("Failed to find layer : %1").arg(name);
  return 0;
}

prim::Layer* LayerManager::getLayer(int n) const
{
  if(n<0 || n>=layers.count()){
    qWarning() << tr("Layer index out of bounds...");
    return 0;
  }
  else
    return layers.at(n);
}

QList<prim::Layer*> LayerManager::getLayers(prim::Layer::LayerType type)
{
  QList<prim::Layer*> layers_found;
  for (prim::Layer *layer : layers)
    if (layer->contentType() == type)
      layers_found.append(layer);
  return layers_found;
}

void LayerManager::setActiveLayer(const QString &name)
{
  for(prim::Layer *layer : layers)
    if(layer->getName() == name){
      setActiveLayer(layer);
      return;
    }

  // no layer had a matching name, do nothing...
  qWarning() << tr("Failed to find layer : %1").arg(name);
}

void LayerManager::setActiveLayer(int n)
{
  if(n<0 || n>=layers.count())
    qWarning() << tr("Layer index out of bounds...");
  else
    setActiveLayer(layers.at(n));
}

void LayerManager::setActiveLayer(prim::Layer *layer)
{
  active_layer = layer;
  // TODO GUI stuff
}

int LayerManager::indexOf(prim::Layer *layer) const
{
  return layer==0 ? layers.indexOf(active_layer) : layers.indexOf(layer);
}

prim::Layer *LayerManager::getMRULayer(prim::Layer::LayerType type)
{
  // find that layer in the MRU hash table
  if (mru_layers.contains(type))
    return mru_layers[type];

  // if no record in MRU, return the first occurance of the type in layers
  for (prim::Layer *layer : layers)
    if (layer->contentType() == type)
      return layer;

  return 0;
}

void LayerManager::saveLayers(QXmlStreamWriter *ws) const
{
  for(prim::Layer *layer : layers)
    layer->saveLayer(ws);
}

void LayerManager::saveLayerItems(QXmlStreamWriter *ws) const
{
  for(prim::Layer *layer : layers){
    ws->writeComment(layer->getName());
    layer->saveItems(ws);
  }
}

// PRIVATE

// initialize widget
void LayerManager::initLayerManager()
{
  // top buttons
  QPushButton *bt_add = new QPushButton(tr("Add")); // TODO implement function

  QHBoxLayout *top_buttons_hl = new QHBoxLayout;
  top_buttons_hl->addWidget(bt_add);

  // grid layout that show all layers
  layer_table = new QTableWidget(this);
  initLayerTableHeaders();

  // Main layout
  QVBoxLayout *main_vl = new QVBoxLayout;
  //main_vl->addLayout(top_buttons_hl); TODO add this back when add function is implemented
  main_vl->addWidget(layer_table);

  setLayout(main_vl);

  // TODO change add/remove to signal based
}


void LayerManager::initDockWidget()
{
  settings::GUISettings *gui_settings = settings::GUISettings::instance();

  // recall or initialise layer dock location
  Qt::DockWidgetArea area;
  area = static_cast<Qt::DockWidgetArea>(gui_settings->get<int>("LAYDOCK/loc"));

  dock_widget = new QDockWidget(tr("Layer Manager"));

  // location behaviour
  dock_widget->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea|Qt::BottomDockWidgetArea);

  // size policy
  dock_widget->setMinimumWidth(gui_settings->get<int>("LAYDOCK/mw"));

  dock_widget->setWidget(this);
}


void LayerManager::initLayerTableHeaders()
{
  qDebug() << "Initializing layer table headers";
  QStringList table_headers;
  // TODO take enum type instead of this stringlist
  table_headers <<
    "ID" <<       // Layer ID, layer's position in layers* stack
    "Type" <<     // Type (lattice, db, electrode) TODO types of default layers can't be changed
    "Name" <<     // Name TODO names of default layers can't be changed
    "Z-Offset" << // Z-Offset (vertical offset from surface) TODO surface zheight can't be changed
    "Z-Height" << // Z-Height (vertical height of layer)
    "" <<  // Visibility
    "";   // Editability

  layer_table->setColumnCount(table_headers.length());
  layer_table->setHorizontalHeaderLabels(table_headers);
  layer_table->setColumnHidden(static_cast<int>(ID), true);
  layer_table->resizeColumnToContents(static_cast<int>(Visibility)); // reduce width of visibility column
  layer_table->resizeColumnToContents(static_cast<int>(Editability)); // reduce width of visibility column

  // header tooltips
  layer_table->horizontalHeaderItem(static_cast<int>(Type))->
      setToolTip("Layer Type: lattice, db, or electrode");
  layer_table->horizontalHeaderItem(static_cast<int>(Name))->
      setToolTip("Layer Name");
  layer_table->horizontalHeaderItem(static_cast<int>(ZOffset))->
      setToolTip("Z-Offset: vertical offset from surface.\nPosition for objects above surface, negative for objects below surface.");
  layer_table->horizontalHeaderItem(static_cast<int>(ZHeight))->
      setToolTip("Z-Height: vertical height of the layer");
  layer_table->horizontalHeaderItem(static_cast<int>(Visibility))->
      setToolTip("Visibility of the layer");
  layer_table->horizontalHeaderItem(static_cast<int>(Editability))->
      setToolTip("Editability of the layer");
}


void LayerManager::populateLayerTable()
{
  clearLayerTable();

  // populate table with layer info
  qDebug() << "Populating layer table";
  for (prim::Layer* layer : layers)
    addLayerRow(layer);

  // signals originating from the table
  connect(layer_table, SIGNAL(cellChanged(int,int)),
            this, SLOT(updateLayerPropFromTable(int,int)));
  // TODO UNDOable z-height change
  // TODO UNDOable layer creation in DesignPanel
}


void LayerManager::refreshLayerTable()
{
  // reload all rows and update with changes, including added/deleted layers
  // TODO
}


void LayerManager::clearLayerTable()
{
  // Delete layer rows and disconnect all signals within the table.
  // Called by destructor on exit or by design panel when loading new file.
  while (!table_row_contents.isEmpty()) {
    LayerTableRowContent *row_content = table_row_contents.takeLast();
    row_content->bt_visibility->disconnect();
    row_content->bt_editability->disconnect();
    delete row_content;
  }
  layer_table->setRowCount(0);  // delete all rows from layer table
}


void LayerManager::updateLayerPropFromTable(int row, int column)
{
  // TODO really need to not hard code layer ID and column position, need some sort of table that translates between readable name and row/col number
  QString layer_name = layer_table->item(row, static_cast<int>(Name))->text();
  //qDebug() << QObject::tr("Row=%1, Col=%2, Layer name=%3").arg(row).arg(column).arg(layer_name);
  prim::Layer* layer = getLayer(layer_name); // get layer according to Layer Name

  switch(column) {
    case static_cast<int>(ID):
      // not supposed to get this
      break;
    case static_cast<int>(Type):
      // not supposed to get this
      break;
    case static_cast<int>(Name):
      // not supposed to get this
      break;
    case static_cast<int>(ZOffset):
      layer->setZOffset(layer_table->item(row, column)->text().toFloat());
      break;
    case static_cast<int>(ZHeight):
      layer->setZHeight(layer_table->item(row, column)->text().toFloat());
      break;
    case static_cast<int>(Visibility):
      // not supposed to get this
      break;
    case static_cast<int>(Editability):
      // not supposed to get this
      break;
    default:
      // not supposed to get this
      break;
  }
  // TODO edit layer property
}


// update widget
void LayerManager::addLayerRow(prim::Layer *layer)
{
  LayerTableRowContent *curr_row_content = new LayerTableRowContent;
  curr_row_content->layer = layer;

  //qDebug() << tr("Constructing layer row GUI elements for layer %1").arg(layer->getName());

  // items that require signal disconnection at removal
  curr_row_content->bt_visibility = new QPushButton(QIcon(":/ico/visible.svg"), "", this);
  curr_row_content->bt_editability = new QPushButton(QIcon(":/ico/editable.svg"), "", this);

  curr_row_content->bt_visibility->setCheckable(true);
  curr_row_content->bt_visibility->setChecked(layer->isVisible());
  curr_row_content->bt_editability->setCheckable(true);
  curr_row_content->bt_editability->setChecked(layer->isActive());

  connect(curr_row_content->bt_visibility, SIGNAL(toggled(bool)), layer, SLOT(visibilityPushButtonChanged(bool)));
  connect(curr_row_content->bt_editability, SIGNAL(toggled(bool)), layer, SLOT(editabilityPushButtonChanged(bool)));

  // other items
  curr_row_content->type = new QTableWidgetItem(layer->contentTypeString());
  curr_row_content->type->setIcon(layerType2Icon(layer->contentType()));
  curr_row_content->type->setToolTip(layer->contentTypeString());

  curr_row_content->name = new QTableWidgetItem(layer->getName());
  curr_row_content->zoffset = new QTableWidgetItem(QString::number(layer->zOffset()));
  curr_row_content->zheight = new QTableWidgetItem(QString::number(layer->zHeight()));

  //qDebug() << QObject::tr("type=%1").arg(curr_row_content->type->text());

  // add to table
  addLayerRow(curr_row_content);
}


void LayerManager::addLayerRow(LayerTableRowContent *row_content)
{
  table_row_contents.append(row_content);

  // add elems to table
  int curr_row = layer_table->rowCount();
  //qDebug() << QObject::tr("curr_row is %1").arg(curr_row);

  //qDebug() << tr("Inserting layer GUI elements to table into row %1").arg(curr_row);

  layer_table->insertRow(curr_row);
  //qDebug() << QObject::tr("type=%1").arg(row_content->type->text());
  layer_table->setItem(curr_row, static_cast<int>(Name), row_content->name);
  // segfault would occur when loading files if type was placed before name...
  layer_table->setItem(curr_row, static_cast<int>(Type), row_content->type);
  layer_table->setItem(curr_row, static_cast<int>(ZOffset), row_content->zoffset);
  layer_table->setItem(curr_row, static_cast<int>(ZHeight), row_content->zheight);

  layer_table->setCellWidget(curr_row, static_cast<int>(Visibility), row_content->bt_visibility);
  layer_table->setCellWidget(curr_row, static_cast<int>(Editability), row_content->bt_editability);
}


QIcon LayerManager::layerType2Icon(const prim::Layer::LayerType layer_type)
{
  // TODO make enumerated layer type instead of hard code string
  if (layer_type == prim::Layer::Lattice)
    return QIcon(":/ico/lattice.svg");
  /*else if (layer_type == prim::Layer::DB)
    return QIcon(":/ico/db.svg");
  else if (layer_type == prim::Layer::Electrode)
    return QIcon(":/ico/electrode.svg");*/
  else
    return QIcon(":/ico/unknown.svg");
}

// TODO
// z-height of db-surface is 0
// +ve for overhanging layers, -ve for buried
// ability to edit z-height for non-surface layers
// option to "always show layer distance from surface"
// states (checkboxes): visible, editable

}