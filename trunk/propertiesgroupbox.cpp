// Copyright (c) 2009 Olivier Crave
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <QtGui/QtGui>

#include "propertiesgroupbox.h"

PropertiesGroupBox::PropertiesGroupBox(QWidget *parent)
    : QGroupBox(tr("Properties"), parent) {

  QGridLayout *layout = new QGridLayout;

  layout->addWidget(new QLabel("Volume:"), 0, 0);
  volume = new QLabel("");
  volume->setAlignment(Qt::AlignRight);
  layout->addWidget(volume, 0, 1);
  layout->addWidget(new QLabel("Surface:"), 1, 0);
  surface = new QLabel("");
  surface->setAlignment(Qt::AlignRight);
  layout->addWidget(surface, 1, 1);
  layout->addWidget(new QLabel("mm^3"), 0, 2);
  layout->addWidget(new QLabel("mm^3"), 1, 2);

  setLayout(layout);
}

PropertiesGroupBox::~PropertiesGroupBox() {}

void PropertiesGroupBox::reset() {
  volume->setText("");
  surface->setText("");
}

void PropertiesGroupBox::setValues(const Entity::Stats stats) {
  QString data;

  data.setNum(stats.volume);
  volume->setText(data);
  //data.setNum(stats.surface);
  //surface->setText(data);
}
