#include <vector>

#include "Core/HW/CPU.h"
#include "VideoCommon/DataReader.h"
#include "VideoCommon/FreeLookReplay.h"
#include "VideoCommon/OpcodeDecoding.h"
#include "VideoCommon/VertexShaderManager.h"
#include "VideoCommon/VideoConfig.h"

namespace FreeLookReplay
{

static std::vector<u8> s_current_frame;
static std::vector<u8> s_last_frame;
static bool s_is_replaying = false;

void RecordCommand(const u8* data, u32 size)
{
	// Don't re-capture our own replayed commands over the frame they came from.
	if (s_is_replaying)
		return;
	s_current_frame.insert(s_current_frame.end(), data, data + size);
}

void EndFrame()
{
	if (s_is_replaying)
		return;
	s_last_frame.swap(s_current_frame);
	s_current_frame.clear();
}

bool IsReplaying()
{
	return s_is_replaying;
}

void Redraw()
{
	// Gating on the toggle is the caller's job (see RequestRedraw) since g_ActiveConfig
	// is only refreshed on Swap, which doesn't happen while paused.
	if (s_last_frame.empty())
		return;

	// FreeLook is applied to the projection matrix, which is only recomputed when marked
	// dirty; force that so the replayed draws pick up the current camera.
	VertexShaderManager::SetProjectionChanged();

	s_is_replaying = true;
	OpcodeDecoder::Run(DataReader(s_last_frame.data(), s_last_frame.data() + s_last_frame.size()), nullptr, false);
	s_is_replaying = false;
}

void RequestRedraw()
{
	if (!g_Config.bFreeLookInstantRender)
		return;

	// The CPU thread also owns the video backend, so the redraw has to happen there
	// instead of on whichever thread called us.
	CPU::RequestFreeLookRedraw();
}

}
