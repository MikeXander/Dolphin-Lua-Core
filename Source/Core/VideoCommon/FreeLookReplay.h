#pragma once

#include "Common/CommonTypes.h"

// Lets FreeLook redraw the last completed frame with the current camera as soon as it
// changes, instead of waiting for the game to submit a new frame (which doesn't happen
// while the game is paused). Works by replaying the last frame's raw GP commands.
//
// Assumes single-core mode, where the CPU thread also owns the video backend.
namespace FreeLookReplay
{
	// CPU thread only. Called from OpcodeDecoder::Run for every executed GP command and
	// from Renderer::RenderToXFB at the end of each frame.
	void RecordCommand(const u8* data, u32 size);
	void EndFrame();

	// CPU thread only. True while Redraw() is replaying commands; lets RenderToXFB force
	// a Swap of its own since the normal VI-timed present doesn't run while paused.
	bool IsReplaying();

	// CPU thread only. Replays the last completed frame's commands.
	void Redraw();

	// Any thread. Wakes the CPU thread to call Redraw(), if the feature is enabled.
	void RequestRedraw();
}
