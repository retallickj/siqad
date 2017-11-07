// @file:     electrode.h
// @author:   Nathan
// @created:  2017.10.27
// @editted:  2017.10.27 - Nathan
// @license:  GNU LGPL v3
//
// @desc:     Electrode objects for creation, deletion, and moving of electrodes.

#ifndef _GUI_PR_ELECTRODE_H_
#define _GUI_PR_ELECTRODE_H_

#include <QtWidgets>
#include "item.h"

namespace prim{

  // forard declarations
  class Layer;

  class Electrode: public prim::Item
  {
  public:

    // constructor, create an electrode given two points
    Electrode(int lay_id, QPointF p1, QPointF p2);
    // destructor
    ~Electrode(){}

    //Electrodes will have two types, clocked (that will change over time) and fixed
    enum ElectrodeType{Clock, Fix};

    ElectrodeType electrode_type;
    //setters
    void setpot(double pot_given);

    // accessors

    QPointF getp1(void){ return p1;}
    QPointF getp2(void){ return p2;}
    QPointF gettopLeft(void){return topLeft;}
    qreal getwidth(void){return elec_width;}
    qreal getheight(void){return elec_height;}
    double getpot(void){return potential;}
    // inherited abstract method implementations

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) Q_DECL_OVERRIDE;

    Item *deepCopy() const;
  //
  protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *e) Q_DECL_OVERRIDE;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e) Q_DECL_OVERRIDE;
    // virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *e) Q_DECL_OVERRIDE;

  private:

    // construct static variables
    void constructStatics();

    // VARIABLES
    QPointF p1;
    QPointF p2;
    QPointF topLeft;

    double potential = 0;
    qreal elec_width;
    qreal elec_height;

    static qreal edge_width;  // proportional width of dot boundary edge

    // static QColor edge_col;   // boundary edge color
    static QColor fill_col;   // dot fill color (same for all lattice dots)

    static qreal in_fill;       // fil factor for inner circle
    static QColor in_fill_col;  // colour of inner circle

    static QColor edge_col;     // edge colour, unselected
    static QColor selected_col; // edge colour, selected
  };

} // end prim namespace

#endif