/*
 * Copyright 2019-present Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 *  This file serves as a helper for bridging folly::coroTask and python
 *  asyncio.future.
 */

#pragma once

#include <Python.h>
#include <folly/Executor.h>
#include <folly/experimental/coro/Task.h>
#include <folly/python/AsyncioExecutor.h>
#include <folly/python/executor_api.h>

namespace folly {
namespace python {

inline folly::Executor* getExecutor() {
  import_folly__executor();
  return get_executor();
}

template <typename T>
void bridgeCoroTask(
    folly::Executor* executor,
    folly::coro::Task<T>&& coroFrom,
    folly::Function<void(folly::Try<T>&&, PyObject*)> callback,
    PyObject* userData) {
  std::move(coroFrom).scheduleOn(executor).start(
      [callback = std::move(callback),
       userData](folly::Try<T>&& result) mutable {
        callback(std::move(result), userData);
      });
}

template <typename T>
void bridgeCoroTask(
    folly::coro::Task<T>&& coroFrom,
    folly::Function<void(folly::Try<T>&&, PyObject*)> callback,
    PyObject* userData) {
  bridgeCoroTask(
      getExecutor(), std::move(coroFrom), std::move(callback), userData);
}

} // namespace python
} // namespace folly
