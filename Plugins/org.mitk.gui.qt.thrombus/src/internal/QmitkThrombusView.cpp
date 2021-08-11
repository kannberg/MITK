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

// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "QmitkThrombusDataReader.h"
#include "QmitkThrombusView.h"
#include "mitkNDIPassiveTool.h"
#include "mitkNDITrackingDevice.h"
#include "mitkNavigationDataDisplacementFilter.h"
#include "mitkNavigationDataRecorder.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>

const std::string QmitkThrombusView::VIEW_ID = "org.mitk.views.thrombus";

void QmitkThrombusView::SetFocus()
{
  // m_Controls.buttonPerformImageProcessing->setFocus();
}

void QmitkThrombusView::OnConnectTrackerBtnSelected()
{
  std::cout << "Generating Source ..." << std::endl;
  mitk::TrackingDeviceSource::Pointer m_Source = mitk::TrackingDeviceSource::New();
  mitk::NDITrackingDevice::Pointer m_Tracker = mitk::NDITrackingDevice::New();
  m_Source->SetTrackingDevice(m_Tracker); // here we set the device for the pipeline source
  m_Source->Connect();                    // here we connect to the tracking system
  // Note we do not call this on the TrackingDevice object
  m_Source->StartTracking(); // start the tracking
                             // Now the source generates outputs.
  
  // Thread to store data in mitk::TrackingDeviceData
}

void QmitkThrombusView::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  // create signal/slot connections
  connect(m_Controls.btnConnectTracker, SIGNAL(clicked()), this, SLOT(OnConnectTrackerBtnSelected()));
}

void QmitkThrombusView::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                           const QList<mitk::DataNode::Pointer> &nodes)
{
  // iterate all selected objects, adjust warning visibility
  foreach (mitk::DataNode::Pointer node, nodes)
  {
    if (node.IsNotNull() && dynamic_cast<mitk::Image *>(node->GetData()))
    {
      m_Controls.labelWarning->setVisible(false);
      return;
    }
  }

  m_Controls.labelWarning->setVisible(true);
}

void QmitkThrombusView::DoImageProcessing()
{
  QList<mitk::DataNode::Pointer> nodes = this->GetDataManagerSelection();
  if (nodes.empty())
    return;

  mitk::DataNode *node = nodes.front();

  if (!node)
  {
    // Nothing selected. Inform the user and return
    QMessageBox::information(nullptr, "Template", "Please load and select an image before starting image processing.");
    return;
  }

  // here we have a valid mitk::DataNode

  // a node itself is not very useful, we need its data item (the image)
  mitk::BaseData *data = node->GetData();
  if (data)
  {
    // test if this data item is an image or not (could also be a surface or something totally different)
    mitk::Image *image = dynamic_cast<mitk::Image *>(data);
    if (image)
    {
      std::stringstream message;
      std::string name;
      message << "Performing image processing for image ";
      if (node->GetName(name))
      {
        // a property called "name" was found for this DataNode
        message << "'" << name << "'";
      }
      message << ".";
      MITK_INFO << message.str();

      // actually do something here...
    }
  }
}
