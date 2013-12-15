/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef GECKO_TASK_TRACER_H
#define GECKO_TASK_TRACER_H

#include "mozilla/RefPtr.h"

class Task;
class nsIRunnable;

namespace mozilla {
namespace tasktracer {

class SourceEventBase : public RefCounted<SourceEventBase>
{
public:
  enum SourceEventType {
    UNKNOWN,
    TOUCH,
    MOUSE,
    POWER_KEY,
    HOME_KEY,
  };

  SourceEventBase();

  virtual ~SourceEventBase() {}

  virtual SourceEventType GetType() const = 0;

  uint64_t mOriginTaskId;
};

/**
 * Create a traced Task to be run by a message loop.
 */
Task *CreateTracedTask(Task *aTask);

/**
 * Create a traced nsIRunnable to be run by an nsThread.
 */
nsIRunnable *CreateTracedRunnable(nsIRunnable *aRunnable);

// XXX Should we expose these two functions?
/**
 * Returns the pointer of currently-traced task id, access from the TracedInfo
 * of current thread.
 */
uint64_t *GetCurrentThreadTaskIdPtr();

void CreateCurrentlyTracedSourceEvent(mozilla::TemporaryRef<SourceEventBase> aSourceEvent);

void SetCurrentlyTracedSourceEvent(SourceEventBase* aSourceEvent);
SourceEventBase* GetCurrentlyTracedSourceEvent();

/**
 * Generates an unique task id for a TeacedRunnable base on its owner thread
 * and the last unique task id.
 */
uint64_t GenNewUniqueTaskId();

/**
 * Free the TracedInfo allocated on its tread local storage.
 */
void FreeTracedInfo();

} // namespace tasktracer
} // namespace mozilla.

#endif
