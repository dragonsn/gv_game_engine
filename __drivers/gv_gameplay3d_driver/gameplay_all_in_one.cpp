#include "stdafx.h"
using namespace gv;
#ifdef  _MSC_VER
#pragma warning(disable: 4002) 
#endif

#if defined (__ANDROID__)
	struct android_app* __state=NULL;
#endif

#include "gameplay3d/src/AbsoluteLayout.hpp"
#include "gameplay3d/src/Animation.hpp"
#include "gameplay3d/src/AnimationClip.hpp"
#include "gameplay3d/src/AnimationController.hpp"
#include "gameplay3d/src/AnimationTarget.hpp"
#include "gameplay3d/src/AnimationValue.hpp"

#if GV_WITH_OPENAL
	#include "gameplay3d/src/AudioBuffer.hpp"
	#include "gameplay3d/src/AudioController.hpp"
	#include "gameplay3d/src/AudioListener.hpp"
	#include "gameplay3d/src/AudioSource.hpp"
#endif

#include "gameplay3d/src/BoundingBox.hpp"
#include "gameplay3d/src/BoundingSphere.hpp"
#include "gameplay3d/src/Bundle.hpp"
#include "gameplay3d/src/Button.hpp"
#include "gameplay3d/src/Camera.hpp"
#include "gameplay3d/src/CheckBox.hpp"
#include "gameplay3d/src/Container.hpp"
#include "gameplay3d/src/Control.hpp"
#include "gameplay3d/src/Curve.hpp"
#include "gameplay3d/src/DebugNew.hpp"
#include "gameplay3d/src/DepthStencilTarget.hpp"
#include "gameplay3d/src/Effect.hpp"
#include "gameplay3d/src/FileSystem.hpp"
#include "gameplay3d/src/FlowLayout.hpp"
#include "gameplay3d/src/Font.hpp"
#include "gameplay3d/src/Form.hpp"
#include "gameplay3d/src/FrameBuffer.hpp"
#include "gameplay3d/src/Frustum.hpp"
#include "gameplay3d/src/Game.hpp"
#include "gameplay3d/src/Gamepad.hpp"
#include "gameplay3d/src/HeightField.hpp"
#include "gameplay3d/src/Image.hpp"
#include "gameplay3d/src/ImageControl.hpp"
#include "gameplay3d/src/Joint.hpp"
#include "gameplay3d/src/Joystick.hpp"
#include "gameplay3d/src/Label.hpp"
#include "gameplay3d/src/Layout.hpp"
#include "gameplay3d/src/Light.hpp"
#if GP_WITH_LOGGER
	#include "gameplay3d/src/Logger.hpp"
#endif
#include "gameplay3d/src/Material.hpp"
#include "gameplay3d/src/MaterialParameter.hpp"
#include "gameplay3d/src/MathUtil.hpp"
#include "gameplay3d/src/Matrix.hpp"
#include "gameplay3d/src/Mesh.hpp"
#include "gameplay3d/src/MeshBatch.hpp"
#include "gameplay3d/src/MeshPart.hpp"
#include "gameplay3d/src/MeshSkin.hpp"
#include "gameplay3d/src/Model.hpp"
#include "gameplay3d/src/Node.hpp"
#include "gameplay3d/src/ParticleEmitter.hpp"
#include "gameplay3d/src/Pass.hpp"

#if GP_WITH_BULLET
	#include "gameplay3d/src/PhysicsCharacter.hpp"
	#include "gameplay3d/src/PhysicsCollisionObject.hpp"
	#include "gameplay3d/src/PhysicsCollisionShape.hpp"
	#include "gameplay3d/src/PhysicsConstraint.hpp"
	#include "gameplay3d/src/PhysicsController.hpp"
	#include "gameplay3d/src/PhysicsFixedConstraint.hpp"
	#include "gameplay3d/src/PhysicsGenericConstraint.hpp"
	#include "gameplay3d/src/PhysicsGhostObject.hpp"
	#include "gameplay3d/src/PhysicsHingeConstraint.hpp"
	#include "gameplay3d/src/PhysicsRigidBody.hpp"
	#include "gameplay3d/src/PhysicsSocketConstraint.hpp"
	#include "gameplay3d/src/PhysicsSpringConstraint.hpp"
	#include "gameplay3d/src/PhysicsVehicle.hpp"
	#include "gameplay3d/src/PhysicsVehicleWheel.hpp"
#endif

#include "gameplay3d/src/Plane.hpp"
#include "gameplay3d/src/Properties.hpp"
#include "gameplay3d/src/Quaternion.hpp"
#include "gameplay3d/src/RadioButton.hpp"
#include "gameplay3d/src/Ray.hpp"
#include "gameplay3d/src/Rectangle.hpp"
#include "gameplay3d/src/Ref.hpp"
#include "gameplay3d/src/RenderState.hpp"
#include "gameplay3d/src/RenderTarget.hpp"
#include "gameplay3d/src/Scene.hpp"
#include "gameplay3d/src/SceneLoader.hpp"
#include "gameplay3d/src/ScreenDisplayer.hpp"

#if GP_WITH_LUA
	#include "gameplay3d/src/ScriptController.hpp"
	#include "gameplay3d/src/ScriptTarget.hpp"
#endif

#include "gameplay3d/src/Slider.hpp"
#include "gameplay3d/src/SpriteBatch.hpp"
#include "gameplay3d/src/Technique.hpp"
#include "gameplay3d/src/Terrain.hpp"
#include "gameplay3d/src/TerrainPatch.hpp"
#include "gameplay3d/src/TextBox.hpp"
#include "gameplay3d/src/Texture.hpp"
#include "gameplay3d/src/Theme.hpp"
#include "gameplay3d/src/ThemeStyle.hpp"
#include "gameplay3d/src/Transform.hpp"
#include "gameplay3d/src/Vector2.hpp"
#include "gameplay3d/src/Vector3.hpp"
#include "gameplay3d/src/Vector4.hpp"
#include "gameplay3d/src/VertexAttributeBinding.hpp"
#include "gameplay3d/src/VertexFormat.hpp"
#include "gameplay3d/src/VerticalLayout.hpp"
