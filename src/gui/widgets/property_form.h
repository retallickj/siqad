// @file:     property_form.h
// @author:   Samuel
// @created:  2018.04.15
// @editted:  2018.04.15  - Samuel
// @license:  GNU LGPL v3
//
// @desc:     Provides a standard QWidget arranging properties into a form for
//            users to edit.

#ifndef _GUI_PROPERTY_FORM_H_
#define _GUI_PROPERTY_FORM_H_

#include <QtWidgets>
#include <QtCore>

#include "../../global.h"
#include "../property_map.h"

namespace gui{

  //! A user-editable form for editing item properties.
  class PropertyForm : public QWidget
  {
    Q_OBJECT

  public:

    //! Constructor
    PropertyForm(PropertyMap pmap, QWidget *parent);
    //PropertyForm(prim::Item *target_item, QWidget *parent);

    //! Destructor
    ~PropertyForm() {}

    //! Return a property map containing all properties in the form, changed or not.
    PropertyMap finalProperties();

    //! Return a property map containing only the changed properties.
    PropertyMap changedProperties();

  private:
    //! Initialize the form
    void initForm();

    PropertyMap pmap;  //! the map where changes will be written to
  };

} // end of gui namespace


#endif  // _GUI_PROPERTY_FORM_H_
