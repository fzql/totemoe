
#include "stdafx.h"
#include "controller.hpp"

void View::Paint(Canvas & canvas, Model & model)
{
    canvas.Text(12, 1, model.GetText(), model.GetLen());
    canvas.Line(10, 0, 10, m_cy);
}