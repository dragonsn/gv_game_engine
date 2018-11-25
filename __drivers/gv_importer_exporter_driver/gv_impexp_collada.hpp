#include "gv_framework_private.h"

#define GVM_WITH_COLLADA 0
#if GVM_WITH_COLLADA

#pragma warning(disable : 4819) // The file contains a character that cannot be
								// represented in the current code page (936).
								// Save the file in Unicode format to prevent
								// data loss

#include "dae.h"
#include "dom/domConstants.h"
#include "dom/domCOLLADA.h"
#include "dae/daeSIDResolver.h"
#include "dae/domAny.h"
#include "dom/domElements.h"

#include "gv_importer_exporter.h"
#include "..\renderer\gv_image.h"
#include "..\renderer\gv_texture.h"
#include "..\renderer\gv_material.h"
#include "..\renderer\gv_index_buffer.h"
#include "..\renderer\gv_vertex_buffer.h"
#include "..\renderer\gv_static_mesh.h"
#include "..\renderer\gv_skeletal_mesh.h"
#include "..\animation\gv_animation.h"
#include "..\renderer\gv_model.h"

#if defined(_DEBUG)
#pragma comment(lib, "libcollada14dom21-sd.lib")
#pragma comment(lib, "pcrecppd.lib")
#pragma comment(lib, "tinyxmld.lib")
#pragma comment(lib, "pcred.lib")
#else
#pragma comment(lib, "libcollada14dom21.lib")
#pragma comment(lib, "pcrecpp.lib")
#pragma comment(lib, "tinyxml.lib")
#pragma comment(lib, "pcre.lib")
#endif
namespace gv
{

class gv_impexp_dae : public gv_importer_exporter
{
public:
	GVM_DCL_CLASS(gv_impexp_dae, gv_importer_exporter);
	gv_impexp_dae()
	{
		link_class(gv_impexp_dae::static_class());
	};
	~gv_impexp_dae(){};
	//=====================================
	struct ani_src
	{
	};

	gv_ani_set* m_current_ani;

	// bool	ReadMaterialLibrary	( domLibrary_materialsRef lib );

	bool ReadImageLibrary(domLibrary_imagesRef lib)
	{
		for (gv_uint i = 0; i < lib->getImage_array().getCount(); i++)
		{
			ReadImage(lib->getImage_array()[i]);
		}
		return true;
	};
	inline gv_texture* GetImage(const char* id)
	{
		if (id == NULL)
			return NULL;
		for (int i = 0; i < m_result_model->m_textures.size(); i++)
		{
			if (m_result_model->m_textures[i]->get_name_id() == id)
				return m_result_model->m_textures[i];
		}
		return NULL;
	}

	gv_texture* ReadImage(domImageRef lib)
	{
		if (lib == NULL)
			return NULL;
		if (lib->getId() == NULL)
			return NULL;
		gv_texture* image = GetImage(lib->getID());
		if (image)
			return image;
		image = m_result_model->get_sandbox()->create_object< gv_texture >(
			gv_id(lib->getID()), m_result_model);
		domImage* imageElement = (domImage*)(domElement*)lib;
		if (imageElement)
		{
			image->set_file_name(
				cdom::uriToNativePath(imageElement->getInit_from()->getValue().str())
					.c_str());
		}
		m_result_model->m_textures.push_back(image);
		return image;
	};
	/*
                  // ------------ Read Animation Libraray ---------------------

                  bool	ReadAnimationLibrary( domLibrary_animationsRef lib )
                  {

                          for ( gv_uint i = 0; i <
     lib->getAnimation_array().getCount(); i++)
                          {
                                  ReadAnimation( lib->getAnimation_array()[i] );
                          }

                          return true;
                  };

                  CrtAnimSrc *ReadAnimationSource( domSourceRef source
     ,gv_ani_track_with_key * ptrack)
                  {
                          if (source==NULL) return NULL;
                          CrtAnimSrc * new_source = CrtNew( CrtAnimSrc );

                          // Set the source name
                          CrtCpy( new_source->ID,  source->getId() );
                          new_source->SetName( new_source->ID );
                          new_source->SetDocURI(
     source->getDocumentURI()->getURI());

                          // Copy over the float array data if any
                          if (source->getFloat_array()) // for float array
                          {
                                  new_source->Count = (gv_uint)
     source->getFloat_array()->getCount();
                                  new_source->Array = CrtNewData( CrtFloat,
     new_source->Count);

                                  daeDoubleArray& floatArray =
     source->getFloat_array()->getValue();

                                  // copy the array data
                                  for ( gv_uint a = 0; a <
     floatArray.getCount(); a++)
                                  {
                                          new_source->Array[a] =
     (CrtFloat)floatArray[a];
                                  }
                          } else if (source->getName_array())
                          {
                                  CrtPrint( "Animation source %s is not
     support\n", new_source->ID );
                          } else {
                                  CrtPrint( "Animation source %s is not
     support\n", new_source->ID );
                          }
                          return new_source;
                  }
                  CrtAnimSampler *ReadAnimationSampler(CrtAnimation * animation,
     domSamplerRef sampler)
                  {
                          if (sampler==NULL) return NULL;
                          CrtAnimSampler * new_sampler = CrtNew( CrtAnimSampler
     );
                          domInputLocal_Array & input_array =
     sampler->getInput_array();
                          for (gv_uint i=0; i<input_array.getCount(); i++)
                          {
                                  domSource * source = (domSource*)
     (domElement*) input_array[i]->getSource().getElement();
                                  new_sampler->inputs[input_array[i]->getSemantic()]
     = animation->Sources[source->getID()];
                          }
                          return new_sampler;
                  }
                  CrtAnimChannel *ReadAnimationChannel(CrtAnimation * animation,
     domChannelRef channel)
                  {
                          if (channel==NULL) return NULL;
                          CrtAnimChannel * new_channel = CrtNew( CrtAnimChannel
     );
                          domSampler * sampler = (domSampler*) (domElement*)
     channel->getSource().getElement();
                          const CrtChar * target = channel->getTarget();
                          new_channel->SetSampler(animation->Samplers[sampler->getID()]);
                          //	new_channel->SetTarget(target.c_str());
                          CrtAnimSrc * input_source =
     animation->Sources[new_channel->Sampler->inputs["INPUT"]->ID];
                          CrtAnimSrc * output_source =
     animation->Sources[new_channel->Sampler->inputs["OUTPUT"]->ID];
                          new_channel->SetInputSrc(input_source);
                          new_channel->SetOutputSrc(output_source);

                          // parse target
                          //	CrtCpy(new_channel->TargetIDStr, target, '/' );
                          // get a pointer to the target element
                          const CrtChar * id_sep = CrtFindStr( target, "/");
                          if (id_sep == NULL)	id_sep = CrtFindStr( target,
     "(");
                          if (id_sep == NULL)	id_sep = CrtFindStr( target,
     ".");

                          // get a pointer to the final element
                          const CrtChar * sid_sep = CrtFindStr( target, "." );
     // search for '.'
                          if (sid_sep == NULL) sid_sep = CrtFindStr( target, "("
     );		// search for '('
                          if (sid_sep == NULL) sid_sep = target +
     CrtLen(target);			// nothing else matches, then there is no
     member

                          if (id_sep != NULL) CrtNCpy(new_channel->TargetIDStr,
     target, (CrtInt) (id_sep - target));
                          if (id_sep != NULL && id_sep !=NULL && sid_sep>id_sep)
     CrtNCpy(new_channel->TargetSIDStr, id_sep+1, (CrtInt) (sid_sep - id_sep -
     1));
                          if (sid_sep!=NULL && (*sid_sep)!='\0')
     CrtCpy(new_channel->TargetMemberStr, sid_sep+1);

                          // resolve target
                          domElement * element = 0;
                          domElement * rootnode =
     channel->getDocument()->getDomRoot();
                          daeSIDResolver sidresolver(rootnode, target);
                          element = sidresolver.getElement();
                          if (element==NULL)
                          {
                                  CrtPrint("Animation target target=%s can not
     be solved\n", target);
                                  CrtDelete( new_channel );
                                  return NULL;
                          }

                          // set channel info
                          COLLADA_TYPE::TypeEnum type =
     element->getElementType();
                          switch(type)
                          {
                          case COLLADA_TYPE::TRANSLATE:
                                  animation->HasTranslation = true;
                                  new_channel->SetTarget( eTranslate );
                                  new_channel->NumElementTargets = 3;
                                  break;
                          case COLLADA_TYPE::ROTATE:
                                  animation->HasRotation = true;
                                  new_channel->SetTarget( eRotate );
                                  new_channel->NumElementTargets = 4;
                                  break;
                          case COLLADA_TYPE::SCALE:
                                  animation->HasScale = true;
                                  new_channel->SetTarget( eScale );
                                  new_channel->NumElementTargets = 3;
                                  break;
                          case COLLADA_TYPE::SOURCE:
                                  animation->HasSource = true;
                                  new_channel->SetTarget( eSource);
                                  new_channel->NumElementTargets = 1;
                                  break;
                          case COLLADA_TYPE::MATRIX:
                                  animation->HasMatrix = true;
                                  new_channel->SetTarget( eMatrix);
                                  new_channel->NumElementTargets = 16;
                                  break;
                          default:
                                  CrtPrint("animation target not supported
     target=%s\n", target);
                                  CrtDelete( new_channel );
                                  return NULL;
                          }

                          // parse member
                          if (new_channel->TargetMemberStr[0] != '\0')
                          {
                                  const CrtChar * target_member =
     new_channel->TargetMemberStr;
                                  if ( CrtICmp( target_member, "AXIS" ) ||
                                          CrtICmp( target_member, "ANGLE" ) )
                                  {
                                          domRotate * rotate = (domRotate *)
     element;
                                          if (rotate->getValue()[0] == 1)
                                          {	// rotate x axis
                                                  animation->HasRotation = true;
                                                  new_channel->SetTarget(eRotXAxis);
                                                  new_channel->NumElementTargets
     = 1;
                                          } else if (rotate->getValue()[1] == 1)
                                          {	// rotate y axis
                                                  animation->HasRotation = true;
                                                  new_channel->SetTarget(eRotYAxis);
                                                  new_channel->NumElementTargets
     = 1;
                                          } else if (rotate->getValue()[2] == 1)
                                          {	// rotate y axis
                                                  animation->HasRotation = true;
                                                  new_channel->SetTarget(eRotZAxis);
                                                  new_channel->NumElementTargets
     = 1;
                                          }
                                  }
                                  else
                                          if ( CrtICmp( target_member, "X" ))
                                          {
                                                  new_channel->SetTarget(eAnimTargetX);
                                                  new_channel->NumElementTargets
     = 1;
                                          }
                                          else
                                                  if ( CrtICmp( target_member,
     "Y" ) )
                                                  {
                                                          new_channel->SetTarget(eAnimTargetY);
                                                          new_channel->NumElementTargets
     = 1;
                                                  }
                                                  else
                                                          if ( CrtICmp(
     target_member, "Z" ) )
                                                          {
                                                                  new_channel->SetTarget(eAnimTargetZ);
                                                                  new_channel->NumElementTargets
     = 1;
                                                          }
                                                          else
                                                                  if (
     (*target_member)>='0' && (*target_member)<='9')
                                                                  {
                                                                          new_channel->SetTarget(eSource);
                                                                          new_channel->NumElementTargets
     = 1;
                                                                  }
                                                                  else
                                                                  {
                                                                          new_channel->SetTarget(eAnimTargetXYZ);
                                                                          new_channel->NumElementTargets
     = 3;
                                                                  }
                          }
                          animation->NumAnimChannels =
     new_channel->NumElementTargets;
                          return new_channel;
                  }
                  gv_ani_set *ReadAnimation( domAnimationRef animation )
                  {
                          if (animation==NULL) return NULL;
                          if ( animation )
                          {
                                  gv_ani_set *
     ani_set=this->m_result_model->get_sandbox()->create_object(gv_id(animation->getId())
     ,m_result_model);
                                  ani_set->set_file_name(
     animation->getDocumentURI()->getURI());

                                  // get the number of sources
                                  domSource_Array & source_array =
     animation->getSource_array();
                                  for (gv_uint i=0; i<source_array.getCount();
     i++)
                                  {
                                          CrtAnimSrc * source =
     ReadAnimationSource(source_array[i]);
                                          if (source)
     newAnim->Sources[source->ID] = source;
                                  }

                                  domSampler_Array & sampler_array =
     animation->getSampler_array();
                                  for (gv_uint s=0; s<sampler_array.getCount();
     s++)
                                  {
                                          CrtAnimSampler * anim_sampler =
     ReadAnimationSampler(newAnim, sampler_array[s]);
                                          if (anim_sampler)
     newAnim->Samplers[sampler_array[s]->getID()] = anim_sampler;
                                  }

                                  domChannel_Array & channel_array =
     animation->getChannel_array();
                                  for (gv_uint i=0; i<channel_array.getCount();
     i++)
                                  {
                                          CrtAnimChannel * channel =
     ReadAnimationChannel(newAnim, channel_array[i]);
                                          if (channel)
     newAnim->Channels.push_back(channel);
                                  }

                                  if (newAnim->Channels.size() > 0)
                                  {
                                          newAnim->GenerateKeys();
                                          Animations.push_back(newAnim);
                                          // also get it's last key time and
     first key time
                                          if( newAnim->GetEndTime() >
     LastKeyTime )
                                                  LastKeyTime =
     newAnim->GetEndTime();
                                  } else
                                  {
                                          CrtPrint("No Channel found in this
     animation\n");
                                          CrtDelete(newAnim);
                                          return NULL;
                                  }
                                  return newAnim;
                          }
                          return NULL;
                  };
         /*
                  CrtMorph *ReadMorph( domMorphRef lib )
                  {
                          const CrtChar * morph_weight_id = NULL;
                          domGeometry * base_dom_geometry = (domGeometry*)
     (domElement*) lib->getSource().getElement();
                          if (base_dom_geometry == NULL)		// base
     geometry no found
                                  return NULL;

                          // base geometry
                          CrtGeometry * base_geometry =
     ReadGeometry(base_dom_geometry);
                          if (base_geometry == NULL)
                                  return NULL;

                          CrtMorph * new_morph = CrtNew(CrtMorph);
                          new_morph->SetBaseGeometry(base_geometry);

                          new_morph->type = CrtController_Morph_Types;

                          // morph method
                          domMorphMethodType method = lib->getMethod();
                          if (method == MORPHMETHODTYPE_NORMALIZED)
                                  new_morph->SetMorphType(MORPH_NORMAL_TYPE);
                          else if (method == MORPHMETHODTYPE_RELATIVE)
                                  new_morph->SetMorphType(MORPH_RELATIVE_TYPE);

                          // parse targets
                          domMorph::domTargetsRef targets = lib->getTargets();
                          domInputLocal_Array & inputs =
     targets->getInput_array();
                          for (size_t i=0; i<inputs.getCount(); i++)
                          {
                                  const CrtChar * semantic =
     inputs[i]->getSemantic();
                                  domSource * source = (domSource*)
     (domElement*) inputs[i]->getSource().getElement();
                                  if (source && semantic)
                                  {
                                          if (CrtICmp(semantic, "MORPH_TARGET"))
                                          {
                                                  domIDREF_array * idref_array =
     source->getIDREF_array();
                                                  domUint count =
     idref_array->getCount();
                                                  xsIDREFS & idrefs =
     source->getIDREF_array()->getValue();
                                                  for (size_t j=0; j<count; j++)
                                                  {
                                                          domGeometry *
     target_geometry_element = (domGeometry*) (domElement*)
     idrefs[j].getElement();
                                                          if
     (target_geometry_element)
                                                          {
                                                                  CrtGeometry *
     base_geometry = ReadGeometry(target_geometry_element);
                                                                  new_morph->targets.push_back(base_geometry);
                                                          }
                                                  }
                                          } else if (CrtICmp(semantic,
     "MORPH_WEIGHT"))
                                          {
                                                  morph_weight_id =
     source->getID();
                                                  domFloat_array * float_array =
     source->getFloat_array();
                                                  domListOfFloats & floats =
     float_array->getValue();
                                                  for (size_t j=0;
     j<float_array->getCount(); j++)
                                                  {
                                                          new_morph->weights.push_back((CrtFloat)
     floats[j]);
                                                  }
                                          } else {
                                                  CrtPrint("ReadMorph
     semantic=%s no understood\n", semantic);
                                          }
                                  }
                          }

                          new_morph->morphed_geometry = CrtNew(CrtGeometry);
                          CrtAssert("No memory\n",
     new_morph->morphed_geometry!=NULL);
                          ParseGeometry(new_morph->morphed_geometry,
     base_dom_geometry);

                          new_morph->Update(-1.0f);

                          // check if there are animation on morph weight
                          for (size_t i=0; i<Animations.size(); i++)
                          {
                                  CrtAnimation * animation = Animations[i];
                                  for (size_t j=0; j<animation->Channels.size();
     j++)
                                  {
                                          CrtAnimChannel * channel =
     animation->Channels[j];
                                          if (CrtICmp(morph_weight_id,
     channel->GetTargetID()))
                                          {
                                                  new_morph->Animations.push_back(animation);
                                          }
                                  }
                          }

                          return new_morph;
                  }

                  CrtController *ReadController( domControllerRef lib )
                  {
                          if (lib->getId()==NULL) return NULL;
                          CrtPrint( "Add new controller %s \n", lib->getId() );
                          CrtController * new_crtcontroller = 0;
                          domMorph * morphElement = lib->getMorph();
                          if (morphElement)
                                  new_crtcontroller = ReadMorph(morphElement);

                          domSkin *skinElement	= lib->getSkin();
                          if (skinElement) // no skin in this controller DO
     LATER, we will support non-skin controller later
                                  new_crtcontroller = ReadSkin(skinElement);

                          if (new_crtcontroller)
                                  Controllers.push_back(new_crtcontroller);

                          return new_crtcontroller;
                  }

                  CrtSkin *ReadSkin( domSkinRef skinElement )
                  {
                          if (skinElement==NULL) // no skin in this controller
     DO LATER, we will support non-skin controller later
                                  return NULL;

                          domController *lib = (domController *)
     skinElement->getParentElement();

                          domElement * source_element =
     skinElement->getSource().getElement();
                          if (source_element==NULL) // skin source not found.
                                  return NULL;
                          if (source_element->getElementType() !=
     COLLADA_TYPE::GEOMETRY){ // skin source is not geometry, DO LATER
                                  CrtPrint("Skin=%s source is not geometry, not
     supported\n", lib->getId());
                                  return NULL;
                          }

                          // Make a CrtSkin to hold this info
                          CrtSkin *newSkin = CrtNew( CrtSkin );
                          CrtAssert("No memory\n", newSkin!=NULL);

                          // set the geometry
                          newSkin->base_geometry = ReadGeometry((domGeometry *)
     source_element);
                          if (newSkin->base_geometry==NULL)	{		//
     target skin is not found
                                  CrtDelete(newSkin);
                                  return NULL;
                          }

                          // Allocate space for the new geometry object
                          newSkin->skinned_geometry     = CrtNew(CrtGeometry);
                          CrtAssert("No memory\n",
     newSkin->skinned_geometry!=NULL);
                          ParseGeometry(newSkin->skinned_geometry,
     (domGeometry*) source_element);

                          newSkin->type = CrtController_Skin_Types;

                          // Get the targetid from the <skin> and set it in
     newController
                          //	char *targetID = (char
     *)skinElement->getSource().getID();
                          //	newSkin->SetTargetId( targetID );
                          newSkin->SetName( lib->getId() );
                          newSkin->SetDocURI( lib->getDocumentURI()->getURI() );

                          if (skinElement->getBind_shape_matrix())
                          {
                                  for (gv_uint m=0; m<16; m++)
                                  {
                                          newSkin->BindShapeMats[m] =
     (CrtFloat)skinElement->getBind_shape_matrix()->getValue()[m];
                                  }
                          }

                          // Find the <joints> and <vertex_weights> elements
     inside <skin>
                          domSkin::domJoints			*jointsElement
     = skinElement->getJoints();
                          domSource
     *jointNamesSource		= NULL;
                          domSource
     *invBindMatsSource		= NULL;

                          // Scan the <joint> looking for specific <input>
     semantics and remember where they are
                          for(gv_uint input = 0; input <
     jointsElement->getInput_array().getCount(); input++)
                          {
                                  if(CrtICmp(jointsElement->getInput_array()[input]->getSemantic(),"JOINT"))
                                  {
                                          // Found the JOINT input semantic
                                          jointNamesSource = (domSource
     *)((daeElement
     *)jointsElement->getInput_array()[input]->getSource().getElement());
                                  }
                                  else
     if(CrtICmp(jointsElement->getInput_array()[input]->getSemantic(),"INV_BIND_MATRIX"))
                                  {
                                          // Found the INV_BIND_MATRIX semantic
                                          invBindMatsSource = (domSource
     *)((daeElement
     *)jointsElement->getInput_array()[input]->getSource().getElement());
                                  }
                          }

                          // Find the inputs we want from <vertex_weights>
                          domSkin::domVertex_weights	*vertexWeightsElement
     = skinElement->getVertex_weights();
                          domSource	*weightsSource	= NULL;

                          // Scan the <vertex_weights> looking for specific
     <input> semantics and remember them
                          for(gv_uint input = 0; input <
     vertexWeightsElement->getInput_array().getCount(); input++)
                          {
                                  if(CrtICmp(vertexWeightsElement->getInput_array()[input]->getSemantic(),"WEIGHT"))
                                  {
                                          // Found the WEIGHT semantic
                                          weightsSource = (domSource
     *)((daeElement
     *)vertexWeightsElement->getInput_array()[input]->getSource().getElement());
                                  }
                          }

                          // Allocate space for the joint names and copy them
                          CrtJoint	*jointNames = 0;
                          gv_uint 	jointCount=0;
                          if (jointNamesSource->getName_array()) {
                                  jointCount = (gv_uint)
     jointNamesSource->getName_array()->getCount();
                                  jointNames = CrtNewData(CrtJoint, jointCount);
                                  CrtAssert("No memory\n", jointNames!=NULL);
                                  for (gv_uint n = 0; n< jointCount; n++)
                                  {
                                          CrtCpy( jointNames[n].Name,
     jointNamesSource->getName_array()->getValue()[n] );
                                  }
                                  newSkin->JointsType =
     CrtJoint::CrtJointsType_SID;
                          } else if (jointNamesSource->getIDREF_array()) {
                                  jointCount	= (gv_uint)
     jointNamesSource->getIDREF_array()->getCount();
                                  jointNames  = CrtNewData(CrtJoint,
     jointCount);
                                  CrtAssert("No memory\n", jointNames!=NULL);
                                  for (gv_uint n = 0; n< jointCount; n++)
                                  {
                                          CrtCpy( jointNames[n].Name,
     jointNamesSource->getIDREF_array()->getValue()[n].getID() );
                                  }
                                  newSkin->JointsType =
     CrtJoint::CrtJointsType_ID;;
                          } else {
                                  CrtAssert("Should never assert here\n",
     false);
                          }
                          // set the JointNames
                          newSkin->Joints		= jointNames;
                          newSkin->JointCount	= jointCount;

                          // Allocate space for the inverse bind matrices and
     copy them
                          gv_uint 	invBindMatsCount	= (gv_uint)
     invBindMatsSource->getFloat_array()->getCount();
                          CrtMatrix	*inverseBindMats	= CrtNewData(
     CrtMatrix, invBindMatsCount/16 );
                          CrtAssert("No memory\n", inverseBindMats!=NULL);
                          CrtFloat	*iBMfloat			= (CrtFloat
     *)inverseBindMats;
                          for (gv_uint m = 0; m< invBindMatsCount; m++)
                          {
                                  iBMfloat[m] =
     (CrtFloat)invBindMatsSource->getFloat_array()->getValue()[m];
                          }
                          // Transpose all of the matrices
                          for (gv_uint m = 0; m < invBindMatsCount/16; m++)
                          {
                                  CrtMatrixTranspose(inverseBindMats[m],inverseBindMats[m]);
                          }
                          // set the InverseBindMats
                          newSkin->AddIBindMats( inverseBindMats,
     invBindMatsCount/16 );
                          newSkin->IBindMatCount = invBindMatsCount/16;
                          CrtDeleteData(inverseBindMats);

                          // Allocate space for the joint weights
                          gv_uint 		vertexWeightsCount	=
     (gv_uint) vertexWeightsElement->getCount();
                          CrtSkinWeight	*weights			=
     CrtNewData( CrtSkinWeight, vertexWeightsCount );
                          CrtAssert("No memory\n", weights!=NULL);
                          // <vcount> tells how many bones are associated with
     each vertex, this indicates how many
                          // pairs of joint/weight indices to process out of the
     <v> array for this vertex.
                          // get pointers to the vcount and v arrays
                          domSkin::domVertex_weights::domVcount *vcountElement
     = vertexWeightsElement->getVcount();
                          domSkin::domVertex_weights::domV	  *vElement
     = vertexWeightsElement->getV();
                          gv_uint vPos = 0;
                          // For each vertex in <vcount>
                          for(gv_uint vertex = 0; vertex < vertexWeightsCount;
     vertex++)
                          {
                                  // Find number of bones (joints/weights) this
     vertex influences and allocate space to store them
                                  gv_uint numInfluences		= (gv_uint)
     vcountElement->getValue()[vertex];
                                  weights[vertex].NumWeights	= numInfluences;

                                  weights[vertex].Weights		=
     CrtNewData( CrtFloat, numInfluences );
                                  CrtAssert("No memory\n",
     weights[vertex].Weights!=NULL);

                                  weights[vertex].Joints		=
     CrtNewData( gv_uint, numInfluences );
                                  CrtAssert("No memory\n",
     weights[vertex].Joints!=NULL);
                                  // For each bone, copy in the joint number and
     the actual float value in the weights (indexed by the
                                  // second value in the <v> array
                                  for(gv_uint inf = 0; inf < numInfluences;
     inf++)
                                  {
                                          weights[vertex].Joints[inf]
     = (gv_uint) vElement->getValue()[vPos++];
                                          weights[vertex].Weights[inf]	=
     (CrtFloat)weightsSource->getFloat_array()->getValue()[(size_t)(vElement->getValue()[vPos++])];
                                  }
                                  weights[vertex].SortWeights();
                          }
                          // set the Weight
                          newSkin->Weights = weights;
                          newSkin->WeightCount = vertexWeightsCount;

                          //	newGeo->Weights = CrtNewData( CrtWeight,
     newGeo->vertexcount );
                          //		for (gv_uint p=0; p<
     newGeo->vertexcount; p++)
                          //			newGeo->Weights[p].Copy(
     &skin->Weights[newGeo->SkinIndex[p]] );


                          return newSkin;
                  };

                  /*
                  bool	ReadEffectLibrary	( domLibrary_effectsRef lib );
                  bool	ReadAnimationLibrary( domLibrary_animationsRef lib );

                  // if object exist, return that object, else
                  // return a pointer to that object if it read and add
     sucessfully, else
                  // return NULL if failed
                  CrtLight		*ReadLight		( domLightRef lib
     );
                  CrtCamera		*ReadCamera		( domCameraRef lib
     );
                  CrtEffect		*ReadEffect		( domEffectRef lib
     );
                  CrtMaterial		*ReadMaterial	( domMaterialRef lib );
                  CrtVoid			ParseGeometry	( CrtGeometry *
     newGeo, domGeometry * dom_geometry);
                  CrtGeometry		*ReadGeometry	( domGeometryRef lib);

                  CrtTexture		*ReadTexture	( domTextureRef lib );
                  CrtAnimation	*ReadAnimation	( domAnimationRef lib );
                  CrtSkin			*ReadSkin		( domSkinRef lib
     );
                  CrtNode			*ReadNode		( domNodeRef
     node, CrtNode * parentNode );
                  CrtMorph		*ReadMorph		( domMorphRef lib
     );
                  CrtController	*ReadController	( domControllerRef lib );

                  CrtAnimSrc		*ReadAnimationSource	( domSourceRef
     source );
                  CrtAnimSampler	*ReadAnimationSampler	(CrtAnimation *
     animation, domSamplerRef sampler);
                  CrtAnimChannel  *ReadAnimationChannel	(CrtAnimation *
     animation, domChannelRef channel);


                  CrtInstanceLight	*ReadInstanceLight( domInstance_lightRef
     lib);
                  CrtInstanceCamera	*ReadInstanceCamera(
     domInstance_cameraRef lib);
                  //	CrtInstanceEffect *ReadInstanceEffect(
     domInstance_effectRef lib);
                  CrtInstanceMaterial *ReadInstanceMaterial(
     domInstance_materialRef lib);
                  CrtInstanceGeometry *ReadInstanceGeometry(
     domInstance_geometryRef lib);
                  //	CrtInstanceImage *ReadInstanceImage(
     domInstance_imageRef lib);
                  //	CrtInstanceTexture *ReadInstanceTexture(
     domInstance_textureRef lib);
                  //	CrtInstanceAnimation *ReadInstanceAnimation(
     domInstance_animationRef lib);
                  CrtInstanceController *ReadInstanceController(
     domInstance_controllerRef lib);

                  CrtChar *ReadCfxBinaryFilename( domExtra_Array &extra_array );

                  //	bool  BuildRenderData( CrtPolyGroup * group,
     domElement * polys, domMesh * mesh, CrtGeoType type, CrtController * ctrl
     );
                  CrtPolygons * BuildPolygons(domPolygons * dom_polygons,
     CrtGeometry * geometry);
                  CrtPolygons * BuildPolygons(domPolylist * dom_polylist,
     CrtGeometry * geometry);
                  CrtTriangles * BuildTriangles(domTriangles * dom_triangles,
     CrtGeometry * geometry);
                  CrtTriStrips * BuildTriStrips(domTristrips * dom_tristrips,
     CrtGeometry * geometry);
                  CrtTriFans * BuildTriFans(domTrifans * dom_trifans,
     CrtGeometry * geometry);
                  CrtLines * BuildLines(domLines * dom_lines, CrtGeometry *
     geometry);
                  CrtLinestrips * BuildLineStrips(domLinestrips *
     dom_linestrips, CrtGeometry * geometry);
                  //	CrtInt BuildPrimitivesData(CrtPolyGroup * primitives,
     domInputLocalOffset_Array & inputs);
                  */

public:
	DAE* m_collada;
	gv_model* m_result_model;

public:
	void get_import_extension(gvt_array< gv_id >& result)
	{
		result.push_back(gv_id_dae);
	};

	gv_class_info* get_import_target_class()
	{
		return gv_model::static_class();
	}

	bool do_import(const gv_string_tmp& file_name, gv_object* target)
	{
		GV_PROFILE_EVENT(gv_impexp_dae__do_import, 0)
		m_result_model = gvt_cast< gv_model >(target);
		if (!m_result_model)
			return false;

		m_collada = new DAE;
		domCOLLADA* dom = m_collada->open(*file_name);
		if (!dom)
		{
			return false;
		}
		// Need to now get the asset tag which will determine what vector x y or z
		// is up.  Typically y or z.
		if (dom->getAsset()->getUp_axis())
		{
			domAsset::domUp_axis* up = dom->getAsset()->getUp_axis();
			switch (up->getValue())
			{
			case UPAXISTYPE_X_UP:
				GVM_LOG(main, "	X Axis is Up axis! default camera is adjusted\n");
				break;
			case UPAXISTYPE_Y_UP:
				GVM_LOG(main, "	Y Axis is Up axis!\n");
				break;
			case UPAXISTYPE_Z_UP:
				GVM_LOG(main, "	Z Axis is Up axis! default camera is adjusted\n");
				break;
			default:

				break;
			}
		}

		// Load all the image libraries
		for (gv_uint i = 0; i < dom->getLibrary_images_array().getCount(); i++)
		{
			ReadImageLibrary(dom->getLibrary_images_array()[i]);
		}
		/*
    // Load all the effect libraries
    //Check for a binary file
    CrtChar *cfxBinFilename = ReadCfxBinaryFilename( dom->getExtra_array() );
    bool success = CrtFalse;
    if ( cfxBinFilename != NULL )
    {
            cfxLoader::setBinaryLoadRemotePath( BasePath );
            success = (bool)
    cfxLoader::loadMaterialsAndEffectsFromBinFile(cfxBinFilename, cfxMaterials,
    cfxEffects, cgContext);
            assert(success);
    }
    else
    {
            success = (bool) cfxLoader::loadMaterialsAndEffects(m_collada,
    cfxMaterials, cfxEffects, cgContext);
            assert(success);
    }
    for ( gv_uint i = 0; i < dom->getLibrary_effects_array().getCount(); i++)
    {
            ReadEffectLibrary( dom->getLibrary_effects_array()[i] );
    }

    // Load all the material libraries
    for ( gv_uint i = 0; i < dom->getLibrary_materials_array().getCount(); i++)
    {
            ReadMaterialLibrary( dom->getLibrary_materials_array()[i] );
    }*/

		// Load all the animation libraries
		/*
    for ( gv_uint i = 0; i < dom->getLibrary_animations_array().getCount(); i++)
    {

            ReadAnimationLibrary( dom->getLibrary_animations_array()[i] );
    }

    // Find the scene we want
    domCOLLADA::domSceneRef domScene = dom->getScene();
    daeElement* defaultScene = NULL;
    if (domScene)
            if (domScene->getInstance_visual_scene())
                    if (domScene->getInstance_visual_scene())
                            defaultScene =
domScene->getInstance_visual_scene()->getUrl().getElement();
    if(defaultScene)
            ReadScene( (domVisual_scene *)defaultScene );

    if (domScene)
    {
#if    !GVM_WITH_BULLET
#else
            size_t count_ips =
domScene->getInstance_physics_scene_array().getCount();
            size_t count_lvs = dom->getLibrary_visual_scenes_array().getCount();
            size_t count_lpm =
dom->getLibrary_physics_models_array().getCount();
            if (count_ips + count_lvs + count_lpm > 2)
            {
                    m_physics = new MyColladaConverter();
    //		bool result = m_physics->load(LFileName);
                    bool result = m_physics->SetColladaDOM(m_collada,
LFileName);
                    if (result)
                    {
                            result = m_physics->convert();
                    } else
                    {
                            _CrtRender.SetUsePhysics(CrtFalse);
                    }
            } else {
                    _CrtRender.SetUsePhysics(CrtFalse);
            }
#endif
    }*/
		return true;
	};

	void get_export_extension(gvt_array< gv_id >& result){

	};
	gv_class_info* get_export_source_class()
	{
		return NULL;
	};
	bool do_export(const gv_string_tmp& file_name, gv_object* source)
	{
		return false;
	};
};

GVM_IMP_CLASS(gv_impexp_dae, gv_importer_exporter)
GVM_END_CLASS
}

#endif