#include <wx/dc.h>
#include <memory>
#include <cmath>
#include "ChartClass.h"
#include "vecmat.h"

ChartClass::ChartClass(std::shared_ptr<ConfigClass> c)
{
    cfg= std::move(c);
    x_step=200;
}

void ChartClass::Set_Range()
{
 double xmin=9999.9,xmax=-9999.9,ymin=9999.9,ymax=-9999.9;
 double x,y,step;
 int i;

 xmin=cfg->Get_x_start();
 xmax=cfg->Get_x_stop();

 step=(cfg->Get_x_stop()-cfg->Get_x_start())/(double)x_step;
 x=cfg->Get_x_start();

 for (i=0;i<=x_step;i++)
  {
   y=GetFunctionValue(x);
   if (y>ymax) ymax=y;
   if (y<ymin) ymin=y;
   x=x+step;
  }

 y_min=ymin;
 y_max=ymax;
 x_min=xmin;
 x_max=xmax;
}

double ChartClass::GetFunctionValue(double x)
{
 if (cfg->Get_F_type()==1) return x*x;
 if (cfg->Get_F_type()==2) return 0.5*exp(4*x-3*x*x);
 return x+sin(x*4.0);
}

double ChartClass::Get_Y_min()
{
    Set_Range();
    return y_min;
}

double ChartClass::Get_Y_max()
{
    Set_Range();
    return y_max;
}

void ChartClass::Draw_Line(Matrix m,wxDC*dc, double x1, double y1, double x2, double y2)
{
    Vector v1,v2;
    v1.data[0]=x1;
    v1.data[1]=y1;
    v1.data[2]=1;
    v2.data[0]=x2;
    v2.data[1]=y2;
    v2.data[2]=1;
    v1=m*v1;
    v2=m*v2;
    dc->DrawLine(v1.data[0],v1.data[1],v2.data[0],v2.data[1]);
}

void ChartClass::Draw_Text(Matrix m,wxDC*dc, double x, double y, wxString text, double angle)
{
    Vector v;
    v.data[0]=x;
    v.data[1]=y;
    v=m*v;
    dc->DrawRotatedText(text,v.data[0],v.data[1],-angle);
}

Matrix ChartClass::Get_Rotation_Matrix(double angle)
{
    Matrix m;
    angle=angle*M_PI/180.0;
    m.data[0][0]=cos(angle);
    m.data[0][1]=-sin(angle);
    m.data[1][0]=sin(angle);
    m.data[1][1]=cos(angle);
    m.data[2][2]=1;
    return m;
}

Matrix ChartClass::Get_Translation_Matrix(double x,double y)
{
    Matrix m;
    m.data[0][0]=1;
    m.data[0][2]=x;
    m.data[1][1]=1;
    m.data[1][2]=y;
    m.data[2][2]=1;
    return m;
}

Matrix ChartClass::Get_Scale_Matrix(double x,double y,double w,double h,double x_min,double x_max,double y_min,double y_max)
{
    Matrix m;
    m.data[0][0]=w/(x_max-x_min);
    m.data[1][1]=-h/(y_max-y_min);
    m.data[2][2]=1;
    return m;
}

void ChartClass::Draw(wxDC *dc, int w, int h)
{
    dc->SetBackground(wxBrush(RGB(255, 255, 255)));
    dc->Clear();
    dc->SetPen(wxPen(RGB(255,0,0)));
    dc->DrawRectangle(10,10,w-20,h-20);
    dc->SetClippingRegion(10,10,w-20,h-20);
    Set_Range();
    Matrix m1;
    double delta_x=(x_max-x_min)/x_step;
    double x0=cfg->Get_x0();
    double y0=cfg->Get_y0();
    double x1=cfg->Get_x1();
    double y1=cfg->Get_y1();
    double alpha=cfg->Get_Alpha();
    bool rotate=cfg->RotateScreenCenter();
    double dx=cfg->Get_dX()/200;
    double dy=cfg->Get_dY()/2;
    double x_min=cfg->Get_x_start();
    double x_max=cfg->Get_x_stop();
    int FunctionType=cfg->Get_F_type();
    double xs =(x1+x0)/2,ys=(y1+y0)/2;
    double xm =(x1-x0),ym=(y1-y0);
    if(rotate) //wyznaczanie macierzy
    {
        m1=Get_Translation_Matrix(w/2,h/2)
        *Get_Rotation_Matrix(alpha)
		*Get_Translation_Matrix(-xs*w/xm,ys*h/ym)
		*Get_Translation_Matrix(dx*w/xm,dy*h/ym)
		*Get_Scale_Matrix(w/2,h/2,w-20,h-20,x0,x1,y0,y1);
    }
    else
    {
		m1=Get_Translation_Matrix(w/2,h/2)
		*Get_Translation_Matrix(dx/xm*w,dy/ym*h)
		*Get_Translation_Matrix(-xs/xm*w+dx,ys/ym*h+dy)
		*Get_Rotation_Matrix(alpha)
		*Get_Scale_Matrix(w/2,h/2,w-20,h-20,x0,x1,y0,y1);
    }
    dc->SetPen(wxPen(RGB(0,0,0)));
    dc->SetTextForeground(wxColour(0,0,0));
    if(y0<0 && y1>0) //rysowanie osi x
    {
        Draw_Line(m1,dc,x_min+dx,dy,x_max+dx,dy);
        Draw_Text(m1,dc,x_min+dx,dy,"x",alpha);
        Draw_Line(m1,dc,x_max+dx,dy,x_max+dx-0.05,dy-0.05);
        Draw_Line(m1,dc,x_max+dx,dy,x_max+dx-0.05,dy+0.05);
        for(double x=x_min;x<x_max+dx;x+=x1/5)
        {
            Draw_Line(m1,dc,x+dx,dy-0.05,x+dx,dy+0.05);
            Draw_Text(m1,dc,x+dx,dy-0.1,wxString::Format("%0.2f",x),alpha);
        }
    }
    if(x0<0 && x1>0) //rysowanie osi y
    {
        Draw_Line(m1,dc,dx,y_min+dy,dx,y_max+dy);
        Draw_Text(m1,dc,dx,y_min+dy,"y",alpha);
        Draw_Line(m1,dc,dx,y_max+dy,dx-0.05,y_max+dy-0.05);
        Draw_Line(m1,dc,dx,y_max+dy,dx+0.05,y_max+dy-0.05);
        for(double y=y_min;y<y_max+dy;y+=y1/5)
        {
            Draw_Line(m1,dc,dx-0.05,y+dy,dx+0.05,y+dy);
            Draw_Text(m1,dc,dx-0.1,y+dy,wxString::Format("%0.2f",y),alpha);
        }
    }
    dc->SetPen(wxPen(RGB(0,0,255)));
    for(double x=x_min+dx;x<=x_max+dx;x+=delta_x) //rysowanie funkcji
    {
        Draw_Line(m1,dc,x,GetFunctionValue(x-dx)+dy,x+delta_x,GetFunctionValue(x+delta_x-dx)+dy);
    }
}
