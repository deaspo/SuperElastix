/*=========================================================================
 *
 *  Copyright Leiden University Medical Center, Erasmus University Medical
 *  Center and contributors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef selxItkSyNImageRegistrationMethodComponent_h
#define selxItkSyNImageRegistrationMethodComponent_h

#include "selxSuperElastixComponent.h"

#include "selxItkRegistrationMethodv4Interfaces.h"
#include "selxSinksAndSourcesInterfaces.h"
#include "selxItkObjectInterfaces.h"

#include "itkSyNImageRegistrationMethod.h"
#include "itkGradientDescentOptimizerv4.h"
#include "itkImageSource.h"
#include "itkTransformToDisplacementFieldFilter.h"
#include "itkComposeDisplacementFieldsImageFilter.h"
#include "itkGaussianExponentialDiffeomorphicTransform.h"
#include "itkGaussianExponentialDiffeomorphicTransformParametersAdaptor.h"

namespace selx
{
template< int Dimensionality, class PixelType, class InternalComputationValueType >
class ItkSyNImageRegistrationMethodComponent :
  public SuperElastixComponent<
  Accepting< itkImageFixedInterface< Dimensionality, PixelType >,
  itkImageMovingInterface< Dimensionality, PixelType >,
  itkMetricv4Interface< Dimensionality, PixelType, InternalComputationValueType >
  >,
  Providing< itkTransformInterface< InternalComputationValueType, Dimensionality >,
  UpdateInterface
  >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef ItkSyNImageRegistrationMethodComponent<
    Dimensionality, PixelType, InternalComputationValueType
    >                                     Self;
  typedef SuperElastixComponent<
    Accepting<
    itkImageFixedInterface< Dimensionality, PixelType >,
    itkImageMovingInterface< Dimensionality, PixelType >,
    itkMetricv4Interface< Dimensionality, PixelType, InternalComputationValueType >
    >,
    Providing< itkTransformInterface< InternalComputationValueType, Dimensionality >,
    UpdateInterface
    >
    >                                     Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkSyNImageRegistrationMethodComponent( const std::string & name, LoggerImpl & logger );
  virtual ~ItkSyNImageRegistrationMethodComponent();

  // Get the type definitions from the interfaces

  using FixedImageType   = typename itkImageFixedInterface< Dimensionality, PixelType >::ItkImageType;
  using MovingImageType  = typename itkImageMovingInterface< Dimensionality, PixelType >::ItkImageType;
  using TransformType    = typename itkTransformInterface< InternalComputationValueType, Dimensionality >::TransformType;
  using TransformPointer = typename itkTransformInterface< InternalComputationValueType, Dimensionality >::TransformPointer;


  typedef itk::SyNImageRegistrationMethod< FixedImageType, MovingImageType >   TheItkFilterType;
  typedef typename TheItkFilterType::ImageMetricType                           ImageMetricType;
  typedef itk::RegistrationParameterScalesFromPhysicalShift< ImageMetricType > ScalesEstimatorType;

  //Accepting Interfaces:
  virtual int Accept( typename itkImageFixedInterface< Dimensionality, PixelType >::Pointer ) override;

  virtual int Accept( typename itkImageMovingInterface< Dimensionality, PixelType >::Pointer ) override;

  virtual int Accept( typename itkMetricv4Interface< Dimensionality, PixelType, InternalComputationValueType >::Pointer ) override;

  //Providing Interfaces:
  virtual TransformPointer GetItkTransform() override;

  virtual void Update() override;

  //BaseClass methods
  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  //static const char * GetName() { return "ItkSyNImageRegistrationMethod"; } ;
  static const char * GetDescription() { return "ItkSyNImageRegistrationMethod Component"; }

private:

  typename TheItkFilterType::Pointer m_theItkFilter;

  // The settings SmoothingSigmas and ShrinkFactors imply NumberOfLevels, if the user
  // provides inconsistent numbers we should detect that and report about it.
  std::string m_NumberOfLevelsLastSetBy;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkSyNImageRegistrationMethodComponent" }, { keys::PixelType, PodString< PixelType >::Get() },
             { keys::InternalComputationValueType, PodString< InternalComputationValueType >::Get() }, { keys::Dimensionality, std::to_string(
                                                                                                           Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkSyNImageRegistrationMethodComponent.hxx"
#endif
#endif // #define selxItkSyNImageRegistrationMethodComponent_h
