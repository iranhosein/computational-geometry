#include "mainwindow.h"

#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QRandomGenerator>
#include <QLabel>
#include <QTime>
#include <QTimer>
#include <iostream>
#include <windows.h>

using namespace std;

class PointSide : public QWidget {
public:
    PointSide(QWidget* parent = nullptr) : QWidget(parent) {}

protected:

    struct Point {
        double x, y;
    };

    // Change this as needed
    int numVertices = 29;

    // String For Label
    std::string labelString = "Not Decide";

    const int WIDTH = width()/2;
    const int HEIGHT = height()/2;

    // Calculate the counterclockwise angle of a point relative to the reference point
    double getCounterClockwiseAngle(const QPointF& p, const QPointF reference) {
        double dx = p.x() - reference.x();
        double dy = p.y() - reference.y();
        return std::atan2(dy, dx);
    }

    // Custom comparison function for sorting vertices counterclockwise
    bool compareVertices(const QPointF& p1, const QPointF& p2, const QPointF& reference) {
        return getCounterClockwiseAngle(p1, reference) < getCounterClockwiseAngle(p2, reference);
    }

    // Algorithm Main
    void search(int i, int j, QPointF selectPoint, vector<QPointF> vertices){
        if(j==i+1){
            labelString = "OutSide";
            return;
        }

        int m = floor((i+j)/2);

        if(compareVertices(vertices[m], selectPoint, vertices[i])==false)
            search(i, m, selectPoint, vertices);
        else if (compareVertices(vertices[j], selectPoint, vertices[m])==false)
            search(m, j, selectPoint, vertices);
        else
        {
            labelString = "InSide";
            return;
        }
    }

    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);

        // Adjust the radius
        int minRadius = std::min(width(), height());
        double circleRadius = QRandomGenerator::global()->bounded(minRadius/2);

        vector<QPointF> vertices;
        for (int i = 0; i < numVertices; ++i) {
            double angle = QRandomGenerator::global()->bounded(2.0 * M_PI);
            double x = circleRadius * cos(angle) +  width()/2;
            double y = circleRadius * sin(angle) + height()/2;
            vertices.push_back({x, y});
        }

        // Calculate the centroid (reference point)
        QPointF centroid = {400, 400};
        for (const auto& vertex : vertices) {
            centroid.setX(centroid.x() + vertex.x());
            centroid.setY(centroid.y() + vertex.y());
        }
        centroid.setX(centroid.x()/vertices.size());
        centroid.setY(centroid.y()/vertices.size());

        // Sort thevertices counterclockwise
        for (int i=0; i<vertices.size() -1; i++){
            for (int j=i+1; j<vertices.size(); j++){
                if (compareVertices(vertices[i], vertices[j], centroid)==false){
                    QPointF point = vertices[i];
                    vertices[i] = vertices[j];
                    vertices[j] = point;
                }
            }
        }

        // Set radius
        int radius = 3;

        for (const auto& vertex : vertices) {
            qreal centerX = vertex.x();
            qreal centerY = vertex.y();

            // Draw Random Points
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColorConstants::Black);
            painter.drawEllipse(centerX - radius, centerY - radius, 2 * radius, 2 * radius);
        }

        // Select Point
        double selectWidth = QRandomGenerator::global()->bounded(2, width()-2);
        double selectHeight = QRandomGenerator::global()->bounded(2, height()-2);
        QPointF selectPoint = QPointF(selectWidth, selectHeight);

        // Draw Select Point
        painter.setBrush(QColorConstants::Red);
        painter.drawEllipse(selectPoint.x() - radius, selectPoint.y() - radius, 2 * radius, 2 * radius);


        // Links Vertices To Eachother
        painter.setPen(Qt::black);
        painter.drawLine(vertices.back(), vertices[0]);
        for (int i=0; i<vertices.size()-1; i++)
            painter.drawLine(vertices[i], vertices[i+1]);

        painter.setPen(Qt::red);
        int i=0;
        int j=vertices.size()-1;
    search:
        if(j==i+1){
            labelString = "OutSide";
        }
        else {
            int m = floor((i+j)/2);

            painter.drawLine(vertices[i], vertices[m]);
            painter.drawLine(vertices[m], vertices[j]);

            if(compareVertices(vertices[m], selectPoint, vertices[i])==false){
                j=m;
                goto search;
            } else if (compareVertices(vertices[j], selectPoint, vertices[m])==false){
                i=m;
                goto search;
            } else {
                labelString = "InSide";
            }
        }
        cout<< endl << labelString << endl;

    }

};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    PointSide widget;
    widget.resize(800, 800);
    widget.show();

    return app.exec();
}
