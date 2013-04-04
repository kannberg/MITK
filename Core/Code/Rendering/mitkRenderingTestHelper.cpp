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

//VTK
#include <vtkRenderWindow.h>
#include <vtkPNGWriter.h>
#include <vtkRenderLargeImage.h>
#include <vtkRenderWindowInteractor.h>

//MITK
#include <mitkRenderingTestHelper.h>
#include <mitkStandaloneDataStorage.h>
#include <mitkRenderWindow.h>
#include <mitkGlobalInteraction.h>
#include <mitkSliceNavigationController.h>
#include <mitkNodePredicateDataType.h>
#include <mitkIOUtil.h>

// include gl to read out properties
#include <vtkOpenGL.h>
#include <vtkOpenGLExtensionManager.h>

//VTK Testing to compare the rendered image pixel-wise against a reference screen shot
#include "vtkTesting.h"

mitkRenderingTestHelper::mitkRenderingTestHelper(int width, int height)
  : m_AutomaticallyCloseRenderWindow(true)
{
  this->Initialize(width, height);
}

mitkRenderingTestHelper::mitkRenderingTestHelper(int width, int height, int argc, char* argv[])
  : m_AutomaticallyCloseRenderWindow(true)
{
  this->Initialize(width, height);
  this->SetInputFileNames(argc, argv);
}

void mitkRenderingTestHelper::Initialize(int width, int height)
{
  // Global interaction must(!) be initialized
  mitk::GlobalInteraction::GetInstance()->Initialize("global");

  m_RenderWindow = mitk::RenderWindow::New();
  m_DataStorage = mitk::StandaloneDataStorage::New();

  m_RenderWindow->GetRenderer()->SetDataStorage(m_DataStorage);
  this->SetMapperIDToRender2D();
  this->GetVtkRenderWindow()->SetSize( width, height );
  m_RenderWindow->GetRenderer()->Resize( width, height);

  //Prints the glinfo after creation of the vtkrenderwindow, we always want to do this for debugging.
  this->PrintGLInfo();
}

mitkRenderingTestHelper::~mitkRenderingTestHelper()
{
}

void mitkRenderingTestHelper::PrintGLInfo()
{
  GLint maxTextureSize;

  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);;

  MITK_INFO << "OpenGL Render Context Information: \n"
            << "- GL_VENDOR: "<< glGetString(GL_VENDOR) << "\n"
            << "- GL_RENDERER: "<< glGetString(GL_RENDERER) << "\n"
            << "- GL_VERSION: "<< glGetString(GL_VERSION)   << "\n"
            << "- GL_MAX_TEXTURE_SIZE: "<< maxTextureSize << "\n"
            << "- GL_EXTENSIONS: "<< glGetString(GL_EXTENSIONS);
}

void mitkRenderingTestHelper::SetMapperID( mitk::BaseRenderer::StandardMapperSlot id)
{
  m_RenderWindow->GetRenderer()->SetMapperID(id);
}

void mitkRenderingTestHelper::SetMapperIDToRender3D()
{
  this->SetMapperID(mitk::BaseRenderer::Standard3D);
}

void mitkRenderingTestHelper::SetMapperIDToRender2D()
{
  this->SetMapperID(mitk::BaseRenderer::Standard2D);
}

void mitkRenderingTestHelper::Render()
{
  //if the datastorage is initialized and at least 1 image is loaded render it
  if(m_DataStorage.IsNotNull() || m_DataStorage->GetAll()->Size() >= 1 )
  {
    //perform global reinit:
    m_RenderWindow->GetRenderer()->PrepareRender();

    //use this to actually show the iamge in a renderwindow
    this->GetVtkRenderWindow()->Render();
    if(m_AutomaticallyCloseRenderWindow == false)
    {
      this->GetVtkRenderWindow()->GetInteractor()->Start();
    }
  }
  else
  {
    MITK_ERROR << "No images loaded in data storage!";
  }

}

void mitkRenderingTestHelper::PrepareRender()
{
  //perform global reinit:
  m_RenderWindow->GetRenderer()->PrepareRender();
}

mitk::DataStorage::Pointer mitkRenderingTestHelper::GetDataStorage()
{
  return m_DataStorage;
}

void mitkRenderingTestHelper::SetInputFileNames(int argc, char* argv[])
{
  // parse parameters
  for (int i = 1; i < argc; ++i)
  {
    //add everything to a list but -T and -V
    std::string tmp = argv[i];
    if((tmp.compare("-T")) && (tmp.compare("-V")))
    {
      this->AddToStorage(tmp);
    }
    else
    {
      break;
    }
  }
}

void mitkRenderingTestHelper::SetViewDirection(mitk::SliceNavigationController::ViewDirection viewDirection)
{
  mitk::BaseRenderer::GetInstance(m_RenderWindow->GetVtkRenderWindow())->GetSliceNavigationController()->SetDefaultViewDirection(viewDirection);
  mitk::RenderingManager::GetInstance()->InitializeViews( m_DataStorage->ComputeBoundingGeometry3D(m_DataStorage->GetAll()) );
}

void mitkRenderingTestHelper::ReorientSlices(mitk::Point3D origin, mitk::Vector3D rotation)
{
  mitk::SliceNavigationController::Pointer sliceNavigationController =
      mitk::BaseRenderer::GetInstance(m_RenderWindow->GetVtkRenderWindow())->GetSliceNavigationController();
  sliceNavigationController->ReorientSlices(origin, rotation);
}

vtkRenderer* mitkRenderingTestHelper::GetVtkRenderer()
{
  return m_RenderWindow->GetRenderer()->GetVtkRenderer();
}

void mitkRenderingTestHelper::SetImageProperty(const char *propertyKey, mitk::BaseProperty* property )
{
  this->m_DataStorage->GetNode(mitk::NodePredicateDataType::New("Image"))->SetProperty(propertyKey, property);
}

vtkRenderWindow* mitkRenderingTestHelper::GetVtkRenderWindow()
{
  return m_RenderWindow->GetVtkRenderWindow();
}

bool mitkRenderingTestHelper::CompareRenderWindowAgainstReference(int argc, char* argv[], double threshold)
{
  this->PrepareRender();

  //retVal meanings: (see VTK/Rendering/vtkTesting.h)
  //0 = test failed
  //1 = test passed
  //2 = test not run
  //3 = something with vtkInteraction
  if(vtkTesting::Test(argc, argv, this->GetVtkRenderWindow(), threshold) == 1)
    return true;
  else
    return false;
}

//method to save a screenshot of the renderwindow (e.g. create a reference screenshot)
void mitkRenderingTestHelper::SaveAsPNG(std::string fileName)
{
  vtkSmartPointer<vtkRenderer> renderer = this->GetVtkRenderer();
  bool doubleBuffering( renderer->GetRenderWindow()->GetDoubleBuffer() );
  renderer->GetRenderWindow()->DoubleBufferOff();

  vtkSmartPointer<vtkRenderLargeImage> magnifier = vtkSmartPointer<vtkRenderLargeImage>::New();
  magnifier->SetInput(renderer);
  magnifier->SetMagnification(1);

  vtkSmartPointer<vtkImageWriter> fileWriter = vtkSmartPointer<vtkPNGWriter>::New();
  fileWriter->SetInput(magnifier->GetOutput());
  fileWriter->SetFileName(fileName.c_str());

  fileWriter->Write();
  renderer->GetRenderWindow()->SetDoubleBuffer(doubleBuffering);
}

void mitkRenderingTestHelper::SaveReferenceScreenShot(std::string fileName)
{
  this->SaveAsPNG(fileName);
}

void mitkRenderingTestHelper::AddToStorage(const std::string &filename)
{
  try
  {
    mitk::DataNode::Pointer node = mitk::IOUtil::LoadDataNode(filename);
    this->AddNodeToStorage(node);
  }
  catch ( itk::ExceptionObject & e )
  {
    MITK_ERROR << "Failed loading test data '" << filename << "': " << e.what();
  }
}

void mitkRenderingTestHelper::AddNodeToStorage(mitk::DataNode::Pointer node)
{
  this->m_DataStorage->Add(node);
  mitk::RenderingManager::GetInstance()->InitializeViews( m_DataStorage->ComputeBoundingGeometry3D(m_DataStorage->GetAll()) );
}
