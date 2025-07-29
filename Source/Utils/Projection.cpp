#include "Projection.hpp"

#include "../SDK/GameClass/ViewRender.hpp"

#include "../Memory.hpp"

#include "Logging.hpp"

#include "BCRL/SafePointer.hpp"
#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/matrix.hpp"

#include "imgui.h"

#include <cstddef>

static ViewRender* view_render = nullptr;
static glm::mat4x4* world_to_projection_matrix = nullptr;

void Projection::resolve_signatures()
{
	view_render
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"restart_in_untrusted">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .next_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"4c 8d 3d">())
			  .add(3)
			  .relative_to_absolute()
			  .expect<ViewRender*>("Couldn't find ViewRender structure");

	Logging::info("Found ViewRender at: {}", view_render);

	//  CRenderGameSystem::GetMatricesForView
	//            (_g_pRenderGameSystem,(CViewSetup *)(CFrustum *)(this + 0x10),(VMatrix *)&g_WorldToView,
	//             (VMatrix *)&g_ViewToProjection,(VMatrix *)&_g_WorldToProjection,
	//             (VMatrix *)&g_WorldToScreen);
	world_to_projection_matrix
		= BCRL::pointer_array(Memory::mem_mgr, view_render, ViewRender::on_render_start_index)
			  .next_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"4c 8d 05">())
			  .add(3)
			  .relative_to_absolute()
			  .expect<glm::mat4x4*>("Couldn't find WorldToProjection matrix");

	Logging::info("Found WorldToProjection matrix at: {}", world_to_projection_matrix);
}

bool Projection::project(const glm::vec3& world, ImVec2& screen)
{
	const glm::mat4x4 matrix = glm::transpose(*world_to_projection_matrix);
	const glm::vec4 transformed = matrix * glm::vec4(world, 1.0F);

	if (transformed.z <= 0.0F || transformed.w <= 0.0F)
		return false;

	const float inv_w = 1.0F / transformed.w;
	const float x_norm = transformed.x * inv_w;
	const float y_norm = transformed.y * inv_w;

	const ImVec2 display_size = ImGui::GetIO().DisplaySize;
	screen.x = display_size.x * 0.5F * (1.0F + x_norm);
	screen.y = display_size.y * 0.5F * (1.0F - y_norm);

	return true;
}

glm::mat4x4* Projection::get_world_to_projection_matrix()
{
	return world_to_projection_matrix;
}
