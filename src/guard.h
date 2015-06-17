/*
Copyright 2015 Gábor Mező aka unbornchikken (gabor.mezo@outlook.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef FIRE_JS_GUARD_H
#define FIRE_JS_GUARD_H

#include <nan.h>

struct Guard
{
    Guard();
    Guard(const Guard&) = delete;
    Guard(Guard&&) = delete;
    ~Guard();
private:
    static uv_mutex_t lock;
    static bool isInitialized;
    static bool Initialize();
};

#endif // FIRE_JS_GUARD_H
