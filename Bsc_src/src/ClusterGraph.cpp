// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#include "ClusterGraph.h"

bool sortInts(int a,	int b){
	return (a < b);
}

listViewCustom::listViewCustom(QWidget *parent): QListWidget(parent){

}

void listViewCustom::dropEvent( QDropEvent * e ) {
	QListWidget::dropEvent(e);
	int count = this->count();
	std::vector<int> sortedClusterIDs;
	for(int index = count - 1;index >= 0;index--)
	{
		QListWidgetItem * item = this->item(index);
		int sortCluster = item->text().toInt();
		sortedClusterIDs.push_back(sortCluster);
		// A wild item has appeared
	}
	sendSortedVector(sortedClusterIDs);
}


ClusterGraph::ClusterGraph(QWidget *parent): QGraphicsView(parent)
{
	ForR = 0;
	pressed = false;
	scale = 1;
	pos = 250;
	win = 1;
	axisPen = QPen(QBrush(Qt::gray),2.0,Qt::DashLine);
	gScene = new QGraphicsScene(this);
	setScene(gScene);
	connect(gScene, SIGNAL(selectionChanged()), this, SLOT(markSelected()));
	//	gScene->setSceneRect(this->rect());
	gScene->setSceneRect(0,0,VIEWWIDTH + 2*OFFSET, CLUSTERGRAPHHEIGHT + 2*OFFSET);

	this->setMouseTracking(true);
	this->setInteractive(true);
	rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
	//	rubberBandSelectionMode();
	updateHisto();
	//	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	heatmap = new QWidget();
	heatmap->hide();
	QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
	this->setSizePolicy(sizePolicy);
	this->setMinimumSize(QSize(0, 0));
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

void ClusterGraph::markSelected(){

	for(int i = 0; i < ellipses->size(); i++){
		QGraphicsEllipseItem* v = ellipses->at(i);

		if(v->isSelected()){
			if(v->rect().width() != RADIUS *4){
				QRectF rect=	v->rect();
				rect.setX(rect.x() - RADIUS);
				rect.setY(rect.y() - RADIUS);
				rect.setWidth(RADIUS *4);
				rect.setHeight(RADIUS *4);
				v->setRect(rect);
				v->setSelected(true);
			}
		}	else{
			if(v->rect().width() == RADIUS *4){
				QRectF rect=	v->rect();
				rect.setX(rect.x() + RADIUS);
				rect.setY(rect.y() + RADIUS);
				rect.setWidth(RADIUS *2);
				rect.setHeight(RADIUS *2);
				v->setRect(rect);
			}
		}
	}

}

//void ClusterGraph::showHeatmap(){
//QWidget* widget = new QWidget(this);
//widget->setObjectName(QString::fromUtf8("Heatmap"));
//widget->setFixedHeight(500);
//widget->setFixedWidth(500);
//QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
//sizePolicy1.setHorizontalStretch(0);
//sizePolicy1.setVerticalStretch(0);
//sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
//widget->setSizePolicy(sizePolicy1);
//
//QWidget* frame = new QWidget();
//QGridLayout* layout = new QGridLayout();
//layout->setMargin(1);
//frame->setLayout(layout);
//widget->setLayout(layout);
//widget->update();
//widget->show();
//}
void ClusterGraph::updateRect()
{
	frontRect.setRight(pos - (500.0*win)/2.0);
	rearRect.setLeft(pos + (500.0*win)/2.0);

	if(frontRect.right()<frontRect.left())
	{
		frontRect.setRight(frontRect.left());
	}

	if(rearRect.right()<rearRect.left())
	{
		rearRect.setLeft(rearRect.right());
	}

	frontRI->setRect(frontRect);
	rearRI->setRect(rearRect);

}

void ClusterGraph::updateHisto()
{
	/*if(osgfile)
	{
	if(stacks!=rVec.size())
	{
	calcStacks();
	}*/

	gScene->clear();
	//gScene->addLine(0 + OFFSET,log((double) 50 + OFFSET),600 + OFFSET,log((double) 50) + OFFSET,axisPen);
	//gScene->addLine(0 + OFFSET,log((double) 100 + OFFSET),600 + OFFSET,log((double) 100) + OFFSET,axisPen);
	//gScene->addLine(0 + OFFSET,log((double) 150 + OFFSET),600 + OFFSET,log((double) 150) + OFFSET,axisPen);
	//gScene->addLine(0 + OFFSET,log((double) 200 + OFFSET),600 + OFFSET,log((double) 200) + OFFSET,axisPen);

	gScene->addLine(0 + OFFSET,((1.0-log10((double) 2))* CLUSTERGRAPHHEIGHT) + OFFSET,VIEWWIDTH + OFFSET,((1.0-log10((double) 2))* CLUSTERGRAPHHEIGHT) + OFFSET,axisPen);
	gScene->addLine(0 + OFFSET,((1.0-log10((double) 4))* CLUSTERGRAPHHEIGHT) + OFFSET,VIEWWIDTH + OFFSET,((1.0-log10((double) 4))* CLUSTERGRAPHHEIGHT) + OFFSET,axisPen);
	gScene->addLine(0 + OFFSET,((1.0-log10((double) 6))* CLUSTERGRAPHHEIGHT) + OFFSET,VIEWWIDTH + OFFSET,((1.0-log10((double) 6))* CLUSTERGRAPHHEIGHT) + OFFSET,axisPen);
	gScene->addLine(0 + OFFSET,((1.0-log10((double) 8))* CLUSTERGRAPHHEIGHT) + OFFSET,VIEWWIDTH + OFFSET,((1.0-log10((double) 8))* CLUSTERGRAPHHEIGHT) + OFFSET,axisPen);
	//	gScene->addLine(0 + OFFSET,((1.0-log10((double) 10))* CLUSTERGRAPHHEIGHT) + OFFSET,600 + OFFSET,((1.0-log10((double) 10))* CLUSTERGRAPHHEIGHT) + OFFSET,axisPen);
	gScene->addLine(0 + OFFSET,CLUSTERGRAPHHEIGHT + OFFSET,VIEWWIDTH + OFFSET,CLUSTERGRAPHHEIGHT + OFFSET,QPen(QBrush(Qt::gray),2.0,Qt::SolidLine));                                                                                                                                                                                                                                                        
	gScene->addLine(OFFSET,OFFSET, OFFSET,CLUSTERGRAPHHEIGHT + OFFSET,QPen(QBrush(Qt::gray),2.0,Qt::SolidLine));
	gScene->addLine(VIEWWIDTH + OFFSET,OFFSET, VIEWWIDTH + OFFSET ,CLUSTERGRAPHHEIGHT + OFFSET, QPen(QBrush(Qt::gray),2.0,Qt::SolidLine));
	//	QPen(QBrush(Qt::gray),1.5,Qt::DashLine);
	//gScene->addLine(0,-75,this->width(),-75,axisPen);
	//gScene->addLine(0,-50,this->width(),-50,axisPen);
	//gScene->addLine(0,0,this->width(),0,axisPen);
	QString str = "Average Distance(mm)";
	gScene->addText(str,QFont("Times",7))->setPos(QPointF(100 + OFFSET,CLUSTERGRAPHHEIGHT + OFFSET));
	gScene->addText("P",QFont("Times",7))->setPos(QPointF(OFFSET - TEXTOFFSET,OFFSET));
	gScene->addText("o",QFont("Times",7))->setPos(QPointF(OFFSET - TEXTOFFSET, (1 * 8) + OFFSET));
	gScene->addText("i",QFont("Times",7))->setPos(QPointF(OFFSET - TEXTOFFSET, (2 * 8) + OFFSET));
	gScene->addText("n",QFont("Times",7))->setPos(QPointF(OFFSET - TEXTOFFSET, (3 * 8) + OFFSET));
	gScene->addText("t",QFont("Times",7))->setPos(QPointF(OFFSET - TEXTOFFSET, (4 * 8) + OFFSET));
	gScene->addText("s",QFont("Times",7))->setPos(QPointF(OFFSET - TEXTOFFSET, (5 * 8) + OFFSET));

	gScene->addText("i",QFont("Times",7))->setPos(QPointF(OFFSET - TEXTOFFSET, (7 * 8) + OFFSET));
	gScene->addText("n",QFont("Times",7))->setPos(QPointF(OFFSET - TEXTOFFSET, (8 * 8) + OFFSET));

	gScene->addText("C",QFont("Times",7))->setPos(QPointF(OFFSET - TEXTOFFSET, (10 * 8) + OFFSET));
	gScene->addText("l",QFont("Times",7))->setPos(QPointF(OFFSET - TEXTOFFSET, (11 * 8) + OFFSET));
	gScene->addText("u",QFont("Times",7))->setPos(QPointF(OFFSET - TEXTOFFSET, (12 * 8) + OFFSET));
	gScene->addText("s",QFont("Times",7))->setPos(QPointF(OFFSET - TEXTOFFSET, (13 * 8) + OFFSET));
	gScene->addText("t",QFont("Times",7))->setPos(QPointF(OFFSET - TEXTOFFSET, (14 * 8) + OFFSET));
	gScene->addText("e",QFont("Times",7))->setPos(QPointF(OFFSET - TEXTOFFSET, (15 * 8) + OFFSET));
	gScene->addText("r",QFont("Times",7))->setPos(QPointF(OFFSET - TEXTOFFSET, (16 * 8) + OFFSET));


	//gScene->addText(str.setNum(75/(float)scale).append("%"),QFont("Times",7))->setPos(QPointF(235,-85));
	//gScene->addText(str.setNum(100/(float)scale).append("%"),QFont("Times",7))->setPos(QPointF(235,-110));




}


void ClusterGraph::mouseReleaseEvent (QMouseEvent* event)
{
	Qt::KeyboardModifiers keyMod = event->modifiers();
	bool isCTRL = keyMod.testFlag(Qt::ControlModifier);

	released = event->pos();
	//	cout << "x:\t" <<  point.x() << "\n" << "y:\t" << point.y() << "\n";
	if(pressed){
		pressed = false;
		//	rubberBand->hide();
		QPainterPath selectionArea;
		QRect test = rubberBand->rect();

		int x1 = 0;
		int x2= 0;
		int y1= 0;
		int y2= 0;

		test.getCoords(&x1, &y1, &x2, &y2);
		if(origin.x() >= released.x()){
			test.translate(origin.x() - test.width(), 0);
			//	x1 = x1 + origin.x() - test.width();
			//x2 = x2 + origin.x() - test.width();
		}else{
			test.translate(origin.x(), 0);
			//x1 = x1 + origin.x();
			//x2 = x2 + origin.x();
		}
		if(origin.y() >= released.y()){
			test.translate(0, origin.y() - test.height());
			/*	y1 = y1 + origin.y() - test.height();
			y2 = y2 + origin.y() - test.height();*/
		}else{
			test.translate(0, origin.y());
			/*y1 = y1 + origin.y();
			y2 = y2 + origin.y();*/
		}
		//	test.setCoords(x1, y1, x2, y2);
		cout << "x1:\t" <<  x1 << "\n" << "x2:\t" << x2 << "\n" << "y1:\t" << y1 << "\n" << "y2:\t" << y2 << "\n";
		selectionArea.addRect(test);

		//selectionArea.addPolygon(mapToScene(rubberBand->rect().normalized()));
		selectedList = gScene->selectedItems();
		gScene->setSelectionArea(selectionArea, this->rubberBandSelectionMode,
			viewportTransform());
		if(isCTRL){
			for(int i = 0; i < selectedList.size(); i++){
				selectedList.at(i)->setSelected(true);
			}
		}
		//gScene->setSelectionArea(selectionArea,
		//	this->rubberBandSelectionMode);

		//QList<QGraphicsItem*> itemList = gScene->selectedItems();
		//for(int i = 0; i < itemList.size(); i++){
		//	QGraphicsEllipseItem* v = dynamic_cast<QGraphicsEllipseItem*>(itemList.at(i));
		//	if(v != 0) {
		//if(!(v->rect().width() == RADIUS *4)){
		//				QRectF rect=	v->rect();
		//				rect.setX(rect.x() - RADIUS);
		//				rect.setY(rect.y() - RADIUS);
		//				rect.setWidth(RADIUS *4);
		//				rect.setHeight(RADIUS *4);
		//				v->setRect(rect);
		//				v->setSelected(true);
		//			}
		//		}
		//	}
	}

}
//pressed = false;
//if(cursor.shape() == Qt::ClosedHandCursor)
//{


//	cursor.setShape(Qt::OpenHandCursor);
//	this->setCursor(cursor);
//}
//ForR = 0;
//mouseMoveEvent(event);
//}


void ClusterGraph::mousePressEvent (QMouseEvent* event)
{	
	origin = event->pos();
	if(cursor.shape() == Qt::PointingHandCursor){
		Qt::KeyboardModifiers keyMod = event->modifiers();
		bool isCTRL = keyMod.testFlag(Qt::ControlModifier);
		QGraphicsItem* pItemUnderMouse = itemAt(origin);
		if(!isCTRL)
			gScene->clearSelection();

		if(pItemUnderMouse){
			QGraphicsEllipseItem* v = dynamic_cast<QGraphicsEllipseItem*>(pItemUnderMouse);
			if(v != 0) {
				v->setSelected(true);
			}
		}
	}else{
		pressed = true;

		if (!rubberBand)
			rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
		rubberBand->setGeometry(QRect(origin, QSize()));
		rubberBand->show();
	}



}
//gScene->setSelectionArea(selectionArea, this->rubberBandSelectionMode,viewportTransform());

/*QList<QGraphicsItem*> itemList = gScene->selectedItems();
for(int i = 0; i < itemList.size(); i++){
QGraphicsEllipseItem* v = dynamic_cast<QGraphicsEllipseItem*>(itemList.at(i));
if(v != 0) {
if(v->isSelected()){
v->setSelected(false);
QRectF rect=	v->rect();
rect.setX(rect.x() + RADIUS);
rect.setY(rect.y() + RADIUS);
rect.setWidth(RADIUS *2);
rect.setHeight(RADIUS *2);
v->setRect(rect);
}
}*/
//}
////QGraphicsView::mousePressEvent(event);
//QPoint eventPos = event->pos();
//pressed = true;


//QGraphicsItem* pItemUnderMouse = itemAt(eventPos);
//if(pItemUnderMouse){
//	QGraphicsEllipseItem* v = dynamic_cast<QGraphicsEllipseItem*>(pItemUnderMouse);
//	if(v != 0) {
//		if(v->isSelected()){
//			v->setSelected(false);
//			QRectF rect=	v->rect();
//			rect.setX(rect.x() + RADIUS);
//			rect.setY(rect.y() + RADIUS);
//			rect.setWidth(RADIUS *2);
//			rect.setHeight(RADIUS *2);
//			v->setRect(rect);
//		}
//		else{
//		//	v->setRect(v->x() - RADIUS, v->y() - RADIUS, RADIUS * 4, RADIUS * 4 );
//		QRectF rect=	v->rect();
//		rect.setX(rect.x() - RADIUS);
//		rect.setY(rect.y() - RADIUS);
//		rect.setWidth(RADIUS *4);
//		rect.setHeight(RADIUS *4);
//		v->setRect(rect);
//		v->setSelected(true);
//		}
//		// old was safely casted to NewType
//		//double x = v->x();
//		//double y = v->y();
//		//v->set
//		//v->setX(x - RADIUS);
//		//v->setY(y - RADIUS);

//	}
//	else{
//		if(cursor.shape() == Qt::OpenHandCursor || cursor.shape() == Qt::SplitHCursor)
//		{

//			if(eventPos.x() <= (frontRect.right()+5))
//			{
//				vectX = eventPos.x() - frontRect.right();
//				ForR = 1;
//			}
//			if(eventPos.x() >= (rearRect.left()-5))
//			{
//				vectX = eventPos.x() - rearRect.left();
//				ForR = 2;
//			}
//			if(eventPos.x() > frontRect.right()+5 && eventPos.x() < rearRect.left()-5)
//			{
//				vectX = eventPos.x() - frontRect.right();
//				ForR = 3;
//				cursor.setShape(Qt::ClosedHandCursor);
//			}
//			this->setCursor(cursor);
//		}
//	
//
//
////	pItemUnderMouse->setScale(2);
//}

//}
//else{
//if(cursor.shape() == Qt::OpenHandCursor || cursor.shape() == Qt::SplitHCursor)
//{

//if(eventPos.x() <= (frontRect.right()+5))
//{
//	vectX = eventPos.x() - frontRect.right();
//	ForR = 1;
//}
//if(eventPos.x() >= (rearRect.left()-5))
//{
//	vectX = eventPos.x() - rearRect.left();
//	ForR = 2;
//}
//if(eventPos.x() > frontRect.right()+5 && eventPos.x() < rearRect.left()-5)
//{
//	vectX = eventPos.x() - frontRect.right();
//	ForR = 3;
//	cursor.setShape(Qt::ClosedHandCursor);
//}
//this->setCursor(cursor);
//}
//}
//}
void ClusterGraph::mouseDoubleClickEvent(QMouseEvent *event){
	//QPoint eventPos = event->pos();
	//
	//if(eventPos.x() > frontRect.right()+5 && eventPos.x() < rearRect.left()-5)
	//{
	//	frontRect = QRectF(0 + OFFSET,0 + OFFSET,0,CLUSTERGRAPHHEIGHT);
	//	rearRect = QRectF(600 + OFFSET,0,0,200);
	//	frontRI->setRect(frontRect);
	//	rearRI->setRect(rearRect);
	//}
	//if(eventPos.x() <= (frontRect.right()+5))
	//{
	//	frontRect = QRectF(0 + OFFSET,0 + OFFSET,0,CLUSTERGRAPHHEIGHT);
	//	frontRI->setRect(frontRect);
	//}
	//if(eventPos.x() >= (rearRect.left()-5))
	//{
	//	rearRect = QRectF(600 + OFFSET,0,0,200);
	//	rearRI->setRect(rearRect);
	//}

}
void ClusterGraph::mouseMoveEvent (QMouseEvent* event)
{
	QPoint point = event->pos();
	if(pressed){

		int x = point.x();
		int y = point.y();
		if(x < 0)
			x = 0;

		if(y < 0)
			y = 0;

		if(x > (VIEWWIDTH + 2* OFFSET))
			x = VIEWWIDTH + 2* OFFSET;

		if(y > (CLUSTERGRAPHHEIGHT+ 2* OFFSET))
			y = (CLUSTERGRAPHHEIGHT + 2* OFFSET);


		rubberBand->setGeometry(QRect(origin, QPoint(x, y)).normalized());
		//		rubberBand->setGeometry(QRect(origin, QPoint(x, y)));
	}
	else{
		QGraphicsItem* pItemUnderMouse = itemAt(point);
		if(pItemUnderMouse){
			QGraphicsEllipseItem* v = dynamic_cast<QGraphicsEllipseItem*>(pItemUnderMouse);
			if(v != 0) {
				cursor.setShape(Qt::PointingHandCursor);
			}else{
				cursor.setShape(Qt::ArrowCursor);
			}
		}else{
			cursor.setShape(Qt::ArrowCursor);
		}
		this->setCursor(cursor);
	}
}
//	QGraphicsView::mouseMoveEvent(event);
//QPainterPath selectionArea;
//	selectionArea.addPolygon(mapToScene( QRect()));
//	gScene->setSelectionArea(selectionArea, this->rubberBandSelectionMode,
//		viewportTransform());
//
//	QList<QGraphicsItem*> itemList = gScene->selectedItems();
//	for(int i = 0; i < itemList.size(); i++){
//		QGraphicsEllipseItem* v = dynamic_cast<QGraphicsEllipseItem*>(itemList.at(i));
//		if(v != 0) {
//					QRectF rect=	v->rect();
//					rect.setX(rect.x() - RADIUS);
//					rect.setY(rect.y() - RADIUS);
//					rect.setWidth(RADIUS *4);
//					rect.setHeight(RADIUS *4);
//					v->setRect(rect);
//					v->setSelected(true);
//			}
//		}
//}

//
////	QGraphicsView::mouseMoveEvent(event);
//	
//	//QGraphicsItem* pItemUnderMouse = itemAt(pMouseEvent->scenePos().x(), pMouseEvent->scenePos().y());
//
//	//if (!pItemUnderMouse)
//	//	return;
//	//if (pItemUnderMouse->isEnabled() &&
//	//	pItemUnderMouse->flags() & QGraphicsItem::ItemIsSelectable)
//	//	pItemUnderMouse->setSelected(!pItemUnderMouse->isSelected());
//
//
//	qreal midDist;
//	QPoint eventPos = event->pos();
//
//
////	else{
//	if(!pressed)
//	{
//		QGraphicsItem* pItemUnderMouse = itemAt(eventPos);
//		if(pItemUnderMouse){
//			QGraphicsEllipseItem* v = dynamic_cast<QGraphicsEllipseItem*>(pItemUnderMouse);
//			if(v != 0) {
//				cursor.setShape(Qt::PointingHandCursor);
//			}
//			else
//			{
//
//		if(eventPos.x() <= (frontRect.right()+5))
//		{
//		cursor.setShape(Qt::SplitHCursor);
//		}
//		if(eventPos.x() >= (rearRect.left()-5))
//		{
//		cursor.setShape(Qt::SplitHCursor);
//		}
//		if(eventPos.x() > frontRect.right()+5 && eventPos.x() < rearRect.left()-5)
//		{
//			cursor.setShape(Qt::OpenHandCursor);	
//		}
//		
//			}
//		}
//		else{
//			if(eventPos.x() <= (frontRect.right()+5))
//			{
//				cursor.setShape(Qt::SplitHCursor);
//			}
//			if(eventPos.x() >= (rearRect.left()-5))
//			{
//				cursor.setShape(Qt::SplitHCursor);
//			}
//			if(eventPos.x() > frontRect.right()+5 && eventPos.x() < rearRect.left()-5)
//			{
//				cursor.setShape(Qt::OpenHandCursor);	
//			}
//		}
//	this->setCursor(cursor);
//	}
//	else
//	{
//		switch (ForR)
//		{
//		case 1:	
//			frontRect.setRight(eventPos.x() - vectX);
//			if(frontRect.right()+5>rearRect.left())
//			{
//				frontRect.setRight(rearRect.left()-10);
//			}
//			if(frontRect.right()<frontRect.left())
//			{
//				frontRect.setRight(frontRect.left());
//			}
//			frontRI->setRect(frontRect);
//		//	emit rectChangedHand(frontRect.right(),rearRect.left());
//		//	emit shaderDataUpdate(frontRect.right()/500,rearRect.left()/500);
//			break;
//		case 2:
//			rearRect.setLeft(eventPos.x() - vectX);
//			if(frontRect.right()+5>rearRect.left())
//			{
//				rearRect.setLeft(frontRect.right()+10);
//			}
//			if(rearRect.right()<rearRect.left())
//			{
//				rearRect.setLeft(rearRect.right());
//			}
//			rearRI->setRect(rearRect);
//		//	emit rectChangedHand(frontRect.right(),rearRect.left());
//		//	emit shaderDataUpdate(frontRect.right()/500,rearRect.left()/500);
//			break;
//		case 3:
//			midDist = rearRect.left() - frontRect.right();
//			if(midDist > 500){
//				midDist = 500;
//			}
//			frontRect.setRight(eventPos.x() - vectX);
//			rearRect.setLeft(frontRect.right() + midDist);
//			frontRI->setRect(frontRect);
//			rearRI->setRect(rearRect);
//		//	emit rectChangedHand(frontRect.right(),rearRect.left());
//		//	emit shaderDataUpdate(frontRect.right()/500,rearRect.left()/500);
//			break;
//		case 0:
//			break;   
//		}
//	}
////	}
//}

void ClusterGraph::paintGraph( std::vector<graphStruct>* coordsVec )
{
	QString str;
	int a = log10((double) 2) * maxSize;
	int b = log10((double) 4) * maxSize;
	int c = log10((double) 6) * maxSize;
	int d = log10((double) 8) * maxSize;
	ellipses = new std::vector<QGraphicsEllipseItem*>();
	coordinations = coordsVec;
	updateHisto();
	gScene->addText(str.setNum(a),QFont("Times",7))->setPos(QPointF((VIEWWIDTH + OFFSET) / 2,((1.0-log10((double) 2))* CLUSTERGRAPHHEIGHT) + OFFSET) );
	gScene->addText(str.setNum(b),QFont("Times",7))->setPos(QPointF((VIEWWIDTH + OFFSET) / 2,((1.0-log10((double) 4))* CLUSTERGRAPHHEIGHT) + OFFSET));
	gScene->addText(str.setNum(c),QFont("Times",7))->setPos(QPointF((VIEWWIDTH + OFFSET) / 2,((1.0-log10((double) 6))* CLUSTERGRAPHHEIGHT) + OFFSET));
	gScene->addText(str.setNum(d),QFont("Times",7))->setPos(QPointF((VIEWWIDTH + OFFSET) / 2,((1.0-log10((double) 8))* CLUSTERGRAPHHEIGHT) + OFFSET));
	double maxDistance = 0;
	//TODO BAD HARD CODED!
	double minDistance = 1000;
	for(int i = 0; i < coordsVec->size(); i++){
		if(maxDistance < coordsVec->at(i).x)
			maxDistance = coordsVec->at(i).x;

		if(minDistance > coordsVec->at(i).x)
			minDistance = coordsVec->at(i).x;
	}

	gScene->addText(str.setNum(maxDistance),QFont("Times",7))->setPos(QPointF( VIEWWIDTH - 15, OFFSET + CLUSTERGRAPHHEIGHT  ));
	gScene->addText(str.setNum(minDistance),QFont("Times",7))->setPos(QPointF(OFFSET - 5 , OFFSET + CLUSTERGRAPHHEIGHT   ));
	double negWidth;
	double posWidth;
	if(maxDistance <= 0){
		negWidth = VIEWWIDTH;
		posWidth = 0;
		//	gScene->addLine( OFFSET, (VIEWHEIGHT + (2*OFFSET)) / 2, OFFSET + VIEWWIDTH, (VIEWHEIGHT + (2*OFFSET)) / 2, Axispen);
	}else if(minDistance >= 0){
		negWidth = 0;
		posWidth = VIEWWIDTH;
	}else if(abs(maxDistance) > abs(minDistance)){
		cout << "Max bigger then Min";
		//double ratio = 1.0 - abs(minDistance / maxDistance);
		double ratio =  abs(maxDistance) / (abs(minDistance) + abs(maxDistance));
		posWidth = (VIEWWIDTH * ratio);
		negWidth = VIEWWIDTH - posWidth;
	}else{
		if(minDistance == 0){
			cout << "BOTH ZERO!";
			posWidth =  VIEWWIDTH / 2;
			negWidth = VIEWWIDTH - posWidth;
		}else{
			cout << "Min bigger then Max";
			double ratio =  abs(maxDistance) / (abs(minDistance) + abs(maxDistance));
			posWidth = (ratio * VIEWWIDTH );
			negWidth = VIEWWIDTH - posWidth;
		}
	}
	gScene->addLine(negWidth,OFFSET, negWidth,CLUSTERGRAPHHEIGHT + OFFSET,QPen(QBrush(Qt::gray),2.0,Qt::SolidLine));
	//gScene->addText(str.setNum(0),QFont("Times",7))->setPos(QPointF(negWidth - 5, 0  ));
	//maxDistance = maxDistance - minDistance;
	for(int i = 0; i < coordsVec->size(); i++){
		QPen pen;  // creates a default pen
		QBrush brush(Qt::SolidPattern);
		int r = coordsVec->at(i).rgb[0] * 255;
		int g = coordsVec->at(i).rgb[1] * 255;
		int b = coordsVec->at(i).rgb[2] * 255;
		brush.setColor(QColor ( r, g, b));
		pen.setBrush(brush);
		//QPen tempPen = QPen(QColor ( coordsVec->at(i).rgb[0] * 255, coordsVec->at(i).rgb[1]* 255, coordsVec->at(i).rgb[2]* 255));


		if(coordsVec->at(i).x < 0){
			coordsVec->at(i).x = ((1.0 - abs(coordsVec->at(i).x /minDistance)) * negWidth) + OFFSET;
		}
		else{
			if(coordsVec->at(i).x == 0){
				if(maxDistance == 0){
					coordsVec->at(i).x = VIEWWIDTH + OFFSET;
				}else{
					if(minDistance == 0){
						coordsVec->at(i).x = OFFSET;
					}
				}

			}
			else{
				coordsVec->at(i).x = ((abs(coordsVec->at(i).x /maxDistance)) * posWidth)+ negWidth + OFFSET;
			}
		}

		//coordsVec->at(i).x = (((coordsVec->at(i).x - minDistance) /maxDistance) * VIEWWIDTH) + OFFSET;
		coordsVec->at(i).y =  CLUSTERGRAPHHEIGHT- (coordsVec->at(i).y * CLUSTERGRAPHHEIGHT) + OFFSET;
		ellipses->push_back(gScene->addEllipse(coordsVec->at(i).x - RADIUS, coordsVec->at(i).y - RADIUS, 2 * RADIUS, 2* RADIUS, QPen(), brush));
		ellipses->at(i)->setFlag(QGraphicsItem::ItemIsSelectable);
		//ellipses->at(i)->setFlag(QGraphicsItem::ItemIsMovable);

	}
}

void ClusterGraph::contextMenuEvent(QContextMenuEvent *event)
{
	QPoint pos = event->pos();
	QGraphicsItem* pItemUnderMouse = itemAt(pos);
	int clusterID;
	bool moreItems = false;
	if(pItemUnderMouse){
		QGraphicsEllipseItem* v = dynamic_cast<QGraphicsEllipseItem*>(pItemUnderMouse);
		if(v != 0) {
			for(int j = 0; j < coordinations->size(); j++){
				QRectF rect = v->rect();
				if((rect.x() + 2* RADIUS+ 2 > coordinations->at(j).x) && (rect.x() + RADIUS * 2 - 2 < coordinations->at(j).x)){
					if(moreItems){
						cout << "\n MORE THAN ONE ITEM IN SAME SPOT!\n" ;
						return;
					}else{
						clusterID = j;
						moreItems = true;
					}
				}
				emit sendClusterID(clusterID);
			}
		}
	}




	//QPoint pos = event->pos();
	//
	//if(heatmap->isHidden())
	//showHeatmap(pos);
}
void ClusterGraph::showHeatmap(QPoint pos ){


	heatmap = new QWidget();
	heatmap->setMinimumHeight(HEATMAPHEIGHT);
	heatmap->setMinimumWidth(HEATMAPWIDTH);
	QPoint pos2;
	int posY = pos.y();
	pos2.setX(pos.x());
	pos2.setY(pos.y()- HEATMAPHEIGHT);
	QPoint globalPos = mapToGlobal(pos);
	cout << "Y-Position: \t" << globalPos.y() << "\n";
	if(globalPos.y() < HEATMAPHEIGHT){
		pos2.setY(pos.y());
	}


	heatmap->move(mapToGlobal(pos2));
	//	widget->move(0, -HEATMAPHEIGHT);
	heatmap->setObjectName(QString::fromUtf8("Heatmap"));
	//TODO Back in
	/*QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(heatmap->sizePolicy().hasHeightForWidth());
	heatmap->setSizePolicy(sizePolicy1);*/

	QWidget* frame = new QWidget();
	QGridLayout* layout = new QGridLayout();
	layout->setMargin(1);
	frame->setLayout(layout);

	heatmap->setLayout(layout);
	for(int x = 0; x < 10 ; x++){
		for(int y = 0; y < 10 ; y++){

			QWidget* wid = new QWidget();
			//	QGraphicsView* gv = new QGraphicsView(wid);
			//	QGraphicsTextItem* ti = new QGraphicsTextItem(gv);
			//	ti->setPlainText("test");
			//QLabel* label = new QLabel(wid);
			//label->setText("Test");
			//label->adjustSize();
			//label->setScaledContents(true);
			QPalette Pal(palette());
			Pal.setColor(QPalette::Background, QColor (((double) x/10.0)*255.0 , ((double) y/10.0)*255.0, 0));
			wid->setAutoFillBackground(true);
			wid->setPalette(Pal);

			/*	QGraphicsView* gv = new QGraphicsView(wid);
			QGraphicsScene* gs = new QGraphicsScene();	
			gs->setSceneRect(0,0,10,10);
			gs->setBackgroundBrush(QBrush(Qt::red, Qt::SolidPattern));
			gv->setScene(gs);*/
			layout->addWidget(wid,x,y);
		}
	}
	heatmap->update();
	heatmap->show();
	calculateHeatmap(pos);
}

void ClusterGraph::calculateHeatmap(QPoint pos ){
	int cluster = 0;

	for(int j = 0; j < coordinations->size(); j++){
		//	QGraphicsEllipseItem* v = dynamic_cast<QGraphicsEllipseItem*>(ui.graphicsView->gScene->selectedItems().at(i));
		if((pos.x() + 2* RADIUS+ 2 > coordinations->at(j).x) && (pos.x() + RADIUS * 2 - 2 < coordinations->at(j).x))
			cluster = j;
	}
	sort(allClusters->at(cluster).begin(), allClusters->at(cluster).end(), sortInts);
	std::vector< std::vector<double> >* heatmap = new std::vector< std::vector<double> >();
	for(int i = 0; i < allClusters->at(cluster).size(); i++){
		std::vector<double> line;
		for(int j = 0; j < allClusters->at(cluster).size(); j++){
			line.push_back(abs((double) (distances->GetValue(i) - distances->GetValue(j))));
		}
		heatmap->push_back(line);
	}
}
//QTimeLine *timer = new QTimeLine(5000);
//   timer->setFrameRange(0, 100);

//   QGraphicsItemAnimation *animation = new QGraphicsItemAnimation;
//   animation->setItem(v);
//   animation->setTimeLine(timer);

//   for (int i = 0; i < 100; i++)
// animation->setScaleAt(i/ 200.0, (1/i) * 4, (1/i) * 4);

//   for (int i = 100; i < 200; i++)
// animation->setScaleAt(i/ 200.0, (1/i) * 0.5, (1/i) * 0.5);
//   //    animation->setPosAt(i / 200.0, QPointF(i, i));
//




//   timer->start();