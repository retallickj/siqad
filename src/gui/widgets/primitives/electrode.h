/** @file:     electrode.h
 *  @author:   Nathan
 *  @created:  2017.10.27
 *  @editted:  2018.01.17 - Nathan
 *  @license:  GNU LGPL v3
 *
 *  @brief:     Function prototypes for the Electrode object.
 */

#ifndef _GUI_PR_ELECTRODE_H_
#define _GUI_PR_ELECTRODE_H_

#include <QtWidgets>
#include "item.h"
#include "resize_frame.h"

namespace prim{

  // forward declarations
  class Layer;

  class Electrode: public prim::Item
  {
  public:
    //! constructor, creates an electrode given two points.
    Electrode(int lay_id, QPointF point1, QPointF point2);
    //! constructor, creates an electrode from the design file.
    Electrode(QXmlStreamReader *ls, QGraphicsScene *scene);
    //! destructor
    ~Electrode(){}
    //! Clock types will vary over time. Fix types are static.
    enum ElectrodeType{Fix, Clock};
    // Initializer for initial attribute values.
    void initElectrode(int lay_id, QPointF point1_in, QPointF point2_in);
    //! Resize according to given coordinates.
    virtual void resize(qreal dx1, qreal dy1, qreal dx2, qreal dy2,
        bool update_handles=false) override;

    // accessors
    QPointF getTopLeft(void){return top_left;}
    QPointF getBotRight(void){return bot_right;}
    qreal getTopDepth(void){return top_depth;}
    qreal getWidth(void) const {return bot_right.x() - top_left.x();}
    qreal getHeight(void) const {return bot_right.y() - top_left.y();}
    qreal getDepth(void){return elec_depth;}

    //! Updates the electrode with its new location. Call this after moving the electrode.
    void updatePoints(QPointF);

    // inherited abstract method implementations
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) Q_DECL_OVERRIDE;
    Item *deepCopy() const override;

    // saving to design
    virtual void saveItems(QXmlStreamWriter *) const override;
    //! Return the class default property map
    virtual gui::PropertyMap *classPropertyMap() override {return &default_class_properties;}
    virtual gui::PropertyMap *classPropertyMap() const override {return &default_class_properties;}

  protected:

    // Mouse events
    virtual QVariant itemChange(GraphicsItemChange change,
        const QVariant &value) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *e) Q_DECL_OVERRIDE;

    // virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e) Q_DECL_OVERRIDE;

  private:
    // construct static variables
    void constructStatics();

    // properties of this item class
    static gui::PropertyMap default_class_properties; //! Default properties for this class
    // VARIABLES
    QPointF top_left; //top left point, since the two points given could be any two opposite points
    QPointF bot_right; //bottom right point, since the two points given could be any two opposite points
    qreal elec_depth;
    qreal top_depth;

    static qreal edge_width;  // proportional width of dot boundary edge
    static QColor fill_col;   // dot fill color (same for all lattice dots)
    static QColor edge_col;     // edge colour, unselected
    static QColor selected_col; // edge colour, selected

    // Resize
    prim::ResizeFrame *resize_frame=0;
    QRectF orig_rect;
  };

} // end prim namespace

#endif
