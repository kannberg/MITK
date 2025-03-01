/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "mitkIOMimeTypes.h"

#include "mitkCustomMimeType.h"
#include "mitkLogMacros.h"

#include "itkGDCMImageIO.h"
#include "itkMetaDataObject.h"

#include <itksys/SystemTools.hxx>
#include <itksys/Directory.hxx>

namespace mitk
{
  IOMimeTypes::BaseDicomMimeType::BaseDicomMimeType(const std::string& name) : CustomMimeType(name)
  {
    this->AddExtension("gdcm");
    this->AddExtension("dcm");
    this->AddExtension("DCM");
    this->AddExtension("dc3");
    this->AddExtension("DC3");
    this->AddExtension("ima");
    this->AddExtension("img");

    this->SetCategory(CATEGORY_IMAGES());
    this->SetComment("DICOM");
  }

  bool IOMimeTypes::BaseDicomMimeType::AppliesTo(const std::string &path) const
  {
    // check whether directory or file
    // if directory try to find first file within it instead
    bool pathIsDirectory = itksys::SystemTools::FileIsDirectory(path);

    std::string filepath = path;

    if (pathIsDirectory)
    {
      itksys::Directory input;
      input.Load(path.c_str());

      std::vector<std::string> files;
      for (unsigned long idx = 0; idx<input.GetNumberOfFiles(); idx++)
      {
        auto filename = input.GetFile(idx);
        if (!itksys::SystemTools::FileIsDirectory(filename) && this->MatchesExtension(filename))
        {
          std::string fullpath = path + "/" + std::string(input.GetFile(idx));
          files.push_back(fullpath.c_str());
        }
      }
      if (!files.empty())
      {
        filepath = files.front();
      }
    }

    // Ask the GDCM ImageIO class directly
    itk::GDCMImageIO::Pointer gdcmIO = itk::GDCMImageIO::New();
    gdcmIO->SetFileName(filepath);
    try {
      gdcmIO->ReadImageInformation();
    }
    catch (const itk::ExceptionObject & /*err*/) {
      return false;
    }

    //DICOMRT modalities have specific reader, don't read with normal DICOM readers
    std::string modality;
    itk::MetaDataDictionary& dict = gdcmIO->GetMetaDataDictionary();
    itk::ExposeMetaData<std::string>(dict, "0008|0060", modality);
    MITK_DEBUG << "DICOM Modality detected by MimeType "<< this->GetName() << " is " << modality;
    if (modality == "RTSTRUCT" || modality == "RTDOSE" || modality == "RTPLAN") {
      return false;
    }
    else {
      return gdcmIO->CanReadFile(filepath.c_str());
    }
  }

  IOMimeTypes::BaseDicomMimeType*IOMimeTypes::BaseDicomMimeType::Clone() const { return new BaseDicomMimeType(*this); }

  IOMimeTypes::DicomMimeType::DicomMimeType() : BaseDicomMimeType(DICOM_MIMETYPE_NAME())
  {
  }

  IOMimeTypes::DicomMimeType* IOMimeTypes::DicomMimeType::Clone() const { return new DicomMimeType(*this); }

  std::vector<CustomMimeType *> IOMimeTypes::Get()
  {
    std::vector<CustomMimeType *> mimeTypes;

    // order matters here (descending rank for mime types)

    mimeTypes.push_back(NRRD_MIMETYPE().Clone());
    mimeTypes.push_back(NIFTI_MIMETYPE().Clone());

    mimeTypes.push_back(VTK_IMAGE_MIMETYPE().Clone());
    mimeTypes.push_back(VTK_PARALLEL_IMAGE_MIMETYPE().Clone());
    mimeTypes.push_back(VTK_IMAGE_LEGACY_MIMETYPE().Clone());

    mimeTypes.push_back(DICOM_MIMETYPE().Clone());

    mimeTypes.push_back(VTK_POLYDATA_MIMETYPE().Clone());
    mimeTypes.push_back(VTK_PARALLEL_POLYDATA_MIMETYPE().Clone());
    mimeTypes.push_back(VTK_POLYDATA_LEGACY_MIMETYPE().Clone());

    mimeTypes.push_back(STEREOLITHOGRAPHY_MIMETYPE().Clone());
    mimeTypes.push_back(WAVEFRONT_OBJ_MIMETYPE().Clone());
    mimeTypes.push_back(STANFORD_PLY_MIMETYPE().Clone());

    mimeTypes.push_back(RAW_MIMETYPE().Clone());
    mimeTypes.push_back(POINTSET_MIMETYPE().Clone());
    return mimeTypes;
  }

  CustomMimeType IOMimeTypes::VTK_IMAGE_MIMETYPE()
  {
    CustomMimeType mimeType(VTK_IMAGE_NAME());
    mimeType.AddExtension("vti");
    mimeType.SetCategory(CATEGORY_IMAGES());
    mimeType.SetComment("VTK Image");
    return mimeType;
  }

  CustomMimeType IOMimeTypes::VTK_IMAGE_LEGACY_MIMETYPE()
  {
    CustomMimeType mimeType(VTK_IMAGE_LEGACY_NAME());
    mimeType.AddExtension("vtk");
    mimeType.SetCategory(CATEGORY_IMAGES());
    mimeType.SetComment("VTK Legacy Image");
    return mimeType;
  }

  CustomMimeType IOMimeTypes::VTK_PARALLEL_IMAGE_MIMETYPE()
  {
    CustomMimeType mimeType(VTK_PARALLEL_IMAGE_NAME());
    mimeType.AddExtension("pvti");
    mimeType.SetCategory(CATEGORY_IMAGES());
    mimeType.SetComment("VTK Parallel Image");
    return mimeType;
  }

  CustomMimeType IOMimeTypes::VTK_POLYDATA_MIMETYPE()
  {
    CustomMimeType mimeType(VTK_POLYDATA_NAME());
    mimeType.AddExtension("vtp");
    mimeType.SetCategory(CATEGORY_SURFACES());
    mimeType.SetComment("VTK PolyData");
    return mimeType;
  }

  CustomMimeType IOMimeTypes::VTK_POLYDATA_LEGACY_MIMETYPE()
  {
    CustomMimeType mimeType(VTK_POLYDATA_LEGACY_NAME());
    mimeType.AddExtension("vtk");
    mimeType.SetCategory(CATEGORY_SURFACES());
    mimeType.SetComment("VTK Legacy PolyData");
    return mimeType;
  }

  CustomMimeType IOMimeTypes::VTK_PARALLEL_POLYDATA_MIMETYPE()
  {
    CustomMimeType mimeType(VTK_PARALLEL_POLYDATA_NAME());
    mimeType.AddExtension("pvtp");
    mimeType.SetCategory(CATEGORY_SURFACES());
    mimeType.SetComment("VTK Parallel PolyData");
    return mimeType;
  }

  CustomMimeType IOMimeTypes::STEREOLITHOGRAPHY_MIMETYPE()
  {
    CustomMimeType mimeType(STEREOLITHOGRAPHY_NAME());
    mimeType.AddExtension("stl");
    mimeType.SetCategory(CATEGORY_SURFACES());
    mimeType.SetComment("Stereolithography");
    return mimeType;
  }

  CustomMimeType IOMimeTypes::WAVEFRONT_OBJ_MIMETYPE()
  {
    CustomMimeType mimeType(WAVEFRONT_OBJ_NAME());
    mimeType.AddExtension("obj");
    mimeType.SetCategory(CATEGORY_SURFACES());
    mimeType.SetComment("Wavefront OBJ");
    return mimeType;
  }

  CustomMimeType IOMimeTypes::STANFORD_PLY_MIMETYPE()
  {
    CustomMimeType mimeType(STANFORD_PLY_NAME());
    mimeType.AddExtension("ply");
    mimeType.SetCategory(CATEGORY_SURFACES());
    mimeType.SetComment("Stanford PLY");
    return mimeType;
  }

  std::string IOMimeTypes::STEREOLITHOGRAPHY_NAME()
  {
    static std::string name = DEFAULT_BASE_NAME() + ".stl";
    return name;
  }

  std::string IOMimeTypes::WAVEFRONT_OBJ_NAME()
  {
    static std::string name = DEFAULT_BASE_NAME() + ".obj";
    return name;
  }

  std::string IOMimeTypes::STANFORD_PLY_NAME()
  {
    static std::string name = DEFAULT_BASE_NAME() + ".ply";
    return name;
  }

  std::string IOMimeTypes::DEFAULT_BASE_NAME()
  {
    static std::string name = "application/vnd.mitk";
    return name;
  }

  std::string IOMimeTypes::CATEGORY_IMAGES()
  {
    static std::string cat = "Images";
    return cat;
  }

  std::string IOMimeTypes::CATEGORY_SURFACES()
  {
    static std::string cat = "Surfaces";
    return cat;
  }

  std::string IOMimeTypes::VTK_IMAGE_NAME()
  {
    static std::string name = DEFAULT_BASE_NAME() + ".vtk.image";
    return name;
  }

  std::string IOMimeTypes::VTK_IMAGE_LEGACY_NAME()
  {
    static std::string name = DEFAULT_BASE_NAME() + ".vtk.image.legacy";
    return name;
  }

  std::string IOMimeTypes::VTK_PARALLEL_IMAGE_NAME()
  {
    static std::string name = DEFAULT_BASE_NAME() + ".vtk.parallel.image";
    return name;
  }

  std::string IOMimeTypes::VTK_POLYDATA_NAME()
  {
    static std::string name = DEFAULT_BASE_NAME() + ".vtk.polydata";
    return name;
  }

  std::string IOMimeTypes::VTK_POLYDATA_LEGACY_NAME()
  {
    static std::string name = DEFAULT_BASE_NAME() + ".vtk.polydata.legacy";
    return name;
  }

  std::string IOMimeTypes::VTK_PARALLEL_POLYDATA_NAME()
  {
    static std::string name = DEFAULT_BASE_NAME() + ".vtk.parallel.polydata";
    return name;
  }

  CustomMimeType IOMimeTypes::NRRD_MIMETYPE()
  {
    CustomMimeType mimeType(NRRD_MIMETYPE_NAME());
    mimeType.AddExtension("nrrd");
    mimeType.AddExtension("nhdr");
    mimeType.SetCategory("Images");
    mimeType.SetComment("NRRD");
    return mimeType;
  }

  CustomMimeType IOMimeTypes::NIFTI_MIMETYPE()
  {
    CustomMimeType mimeType(NIFTI_MIMETYPE_NAME());
    mimeType.AddExtension("nii");
    mimeType.AddExtension("nii.gz");
    mimeType.AddExtension("hdr");
    mimeType.AddExtension("hdr.gz");
    mimeType.AddExtension("img");
    mimeType.AddExtension("img.gz");
    mimeType.AddExtension("nia");
    mimeType.SetCategory("Images");
    mimeType.SetComment("Nifti");
    return mimeType;
  }

  CustomMimeType IOMimeTypes::RAW_MIMETYPE()
  {
    CustomMimeType mimeType(RAW_MIMETYPE_NAME());
    mimeType.AddExtension("raw");
    mimeType.SetCategory("Images");
    mimeType.SetComment("Raw data");
    return mimeType;
  }

  IOMimeTypes::DicomMimeType IOMimeTypes::DICOM_MIMETYPE() { return DicomMimeType(); }
  std::string IOMimeTypes::NRRD_MIMETYPE_NAME()
  {
    static std::string name = DEFAULT_BASE_NAME() + ".image.nrrd";
    return name;
  }

  std::string IOMimeTypes::NIFTI_MIMETYPE_NAME()
  {
    static std::string name = DEFAULT_BASE_NAME() + ".image.nifti";
    return name;
  }

  std::string IOMimeTypes::RAW_MIMETYPE_NAME()
  {
    static std::string name = DEFAULT_BASE_NAME() + ".image.raw";
    return name;
  }

  std::string IOMimeTypes::DICOM_MIMETYPE_NAME()
  {
    static std::string name = DEFAULT_BASE_NAME() + ".image.dicom";
    return name;
  }

  CustomMimeType IOMimeTypes::POINTSET_MIMETYPE()
  {
    CustomMimeType mimeType(POINTSET_MIMETYPE_NAME());
    mimeType.AddExtension("mps");
    mimeType.SetCategory("Point Sets");
    mimeType.SetComment("MITK Point Set");
    return mimeType;
  }

  std::string IOMimeTypes::POINTSET_MIMETYPE_NAME()
  {
    static std::string name = DEFAULT_BASE_NAME() + ".pointset";
    return name;
  }

  CustomMimeType IOMimeTypes::GEOMETRY_DATA_MIMETYPE()
  {
    mitk::CustomMimeType mimeType(DEFAULT_BASE_NAME() + ".geometrydata");
    mimeType.AddExtension("mitkgeometry");
    mimeType.SetCategory("Geometries");
    mimeType.SetComment("GeometryData object");
    return mimeType;
  }
}
