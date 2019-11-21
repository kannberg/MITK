/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef QmitkExtFileSaveProjectAction_H_
#define QmitkExtFileSaveProjectAction_H_

#include <QAction>

#include <org_mitk_gui_qt_flowbenchapplication_Export.h>

#include <berrySmartPointer.h>

namespace berry {
struct IWorkbenchWindow;
}

class MITK_QT_FLOW_BENCH_APP_EXPORT QmitkExtFileSaveProjectAction : public QAction
{
  Q_OBJECT

public:

  QmitkExtFileSaveProjectAction(berry::SmartPointer<berry::IWorkbenchWindow> window);
  QmitkExtFileSaveProjectAction(berry::IWorkbenchWindow* window);

protected slots:

  void Run();

private:

  void Init(berry::IWorkbenchWindow* window);

  berry::IWorkbenchWindow* m_Window;
};


#endif /*QmitkExtFileSaveProjectAction_H_*/
