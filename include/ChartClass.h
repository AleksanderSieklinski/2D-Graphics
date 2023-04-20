#ifndef __ChartClass__
#define __ChartClass__

#include <memory>
#include "ConfigClass.h"
class Matrix;

class ChartClass
{
    private:
        std::shared_ptr<ConfigClass> cfg;
        int x_step;         // liczba odcinkow z jakich bedzie sie skladal wykres
        double x_min,x_max; // zakres zmiennej x
        double y_min,y_max; // zakres wartosci przyjmowanych przez funkcje
        double GetFunctionValue(double x); // zwraca wartosci rysowanej funkcji
        static void Draw_Line(Matrix m,wxDC*dc, double x1, double y1, double x2, double y2);
        // rysuje odcinek o koncach (x1,y1) i (x2,y2) po transformacji m
        static void Draw_Text(Matrix m,wxDC*dc, double x, double y, wxString text, double angle);
        // rysuje tekst text w punkcie (x,y) po transformacji m
        static Matrix Get_Rotation_Matrix(double angle);
        // zwraca macierz rotacji 2D
        static Matrix Get_Translation_Matrix(double x,double y);
        // zwraca macierz translacji 2D
        static Matrix Get_Scale_Matrix(double x,double y,double w,double h,double x_min,double x_max,double y_min,double y_max);
        // zwraca macierz skalowania 2D
    public:
        ChartClass(std::shared_ptr<ConfigClass> c);
        void Set_Range();   // ustala wartosci zmiennych x_min,y_min,x_max,y_max
        double Get_Y_min(); // zwraca y_min
        double Get_Y_max(); // zwraca y_max
        void Draw(wxDC *dc, int w, int h);  // rysuje wykres
};

#endif
