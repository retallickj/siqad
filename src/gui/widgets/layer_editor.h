// @file:     layer_editor.h
// @author:   Samuel
// @created:  2017.12.22
// @editted:  2017.12.22 - Samuel
// @license:  GNU LGPL v3
//
// @desc:     edit layer properties

#ifndef _GUI_LAYER_EDITOR_H_
#define _GUI_LAYER_EDITOR_H_

#include <QtWidgets>

#include "design_panel.h"
#include "primitives/items.h"
#include "primitives/layer.h"

namespace gui{

  class LayerEditor : public QWidget
  {
    Q_OBJECT
  public:
    // constructor
    LayerEditor(gui::DesignPanel *design_pan, QWidget *parent);

    // destructor
    ~LayerEditor() {};

    // GUI items in layer list grid
    struct layerGridRowItems
    {
      QLabel *label_layer_name;     // layer name
      QLabel *label_content_type;   // layer content type
      QLineEdit *le_zheight;        // layer z-height
      QCheckBox *cb_visibility;     // layer visibility
    };

    void updateLayerList();

  private:
    void initLayerEditor();

    void addLayerRow(); // add an empty row to layer list

    // vars
    gui::DesignPanel *dp;
    QStack<prim::Layer*> *layers;

    // GUI
    QVBoxLayout *layer_list_vl;
    QGridLayout *layer_grid_l;
    QStack<layerGridRowItems> layer_grid_items;
  };

}


#endif