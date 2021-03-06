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

#include "ext.h"
#include "createarray.h"
#include "helpers.h"
#include "arraywrapper.h"
#include "errors.h"
#include "guard.h"

using namespace v8;
using namespace std;
using namespace node;

// TODO: RND seed functions, when 3.0 gets released

NAN_METHOD(RandU)
{
    NanScope();

    try
    {
        ARGS_LEN(2);
        auto dimAndType = ParseDimAndTypeArgs(args);
        Guard();
        NanReturnValue(ArrayWrapper::New(af::randu(dimAndType.first, dimAndType.second)));
    }
    FIRE_CATCH
}

NAN_METHOD(RandN)
{
    NanScope();

    try
    {
        ARGS_LEN(2);
        auto dimAndType = ParseDimAndTypeArgs(args);
        if (dimAndType.second == f32 || dimAndType.second == f64)
        {
            Guard();
            NanReturnValue(ArrayWrapper::New(af::randn(dimAndType.first, dimAndType.second)));
            return;
        }
        return NAN_THROW_INVALID_DTYPE();
    }
    FIRE_CATCH
}

NAN_METHOD(Identity)
{
    NanScope();

    try
    {
        ARGS_LEN(2);
        auto dimAndType = ParseDimAndTypeArgs(args);
        Guard();
        NanReturnValue(ArrayWrapper::New(af::identity(dimAndType.first, dimAndType.second)));
    }
    FIRE_CATCH
}

NAN_METHOD(Range)
{
    NanScope();

    try
    {
        ARGS_LEN(3);
        auto dimAndType = ParseDimAndTypeArgs(args, -1, 1);
        af_dtype seqDim = (af_dtype)args[args.Length() - 2]->Uint32Value();
        Guard();
        NanReturnValue(ArrayWrapper::New(af::range(dimAndType.first, seqDim, dimAndType.second)));
    }
    FIRE_CATCH
}

NAN_METHOD(Iota)
{
    NanScope();

    try
    {
        ARGS_LEN(3);
        auto dims = ToDim4(args[0]);
        auto titleDims = ToDim4(args[1]);
        auto type = GetDTypeInfo(args[3]->Uint32Value());
        Guard();
        NanReturnValue(ArrayWrapper::New(af::iota(dims, titleDims, type.first)));
    }
    FIRE_CATCH
}

NAN_METHOD(Diag)
{
    NanScope();

    try
    {
        ARGS_LEN(1);
        auto pArray = ArrayWrapper::GetArrayAt(args, 0);
        int num = 0;
        bool extract = true;
        if (args.Length() > 1)
        {
            num = args[1]->Int32Value();
        }
        if (args.Length() > 2)
        {
            extract = args[2]->BooleanValue();
        }
        Guard();
        NanReturnValue(ArrayWrapper::New(af::diag(*pArray, num, extract)));
    }
    FIRE_CATCH
}

NAN_METHOD(Constant)
{
    NanScope();

    // Notice: In v8 we can go for double, complex, and int64 as a string, because v8 numbers are doubles.

    try
    {
        ARGS_LEN(3);
        auto dimAndType = ParseDimAndTypeArgs(args, -1, 0, 1);
        auto value = args[0];
        Guard();
        if (value->IsNumber())
        {
            double v = value->NumberValue();
            switch (dimAndType.second)
            {
                case f32:
                    NanReturnValue(ArrayWrapper::New(af::constant<float>(v, dimAndType.first, dimAndType.second)));
                    return;
                case f64:
                    NanReturnValue(ArrayWrapper::New(af::constant<double>(v, dimAndType.first, dimAndType.second)));
                    return;
                case s32:
                    NanReturnValue(ArrayWrapper::New(af::constant<int>(v, dimAndType.first, dimAndType.second)));
                    return;
                case u32:
                    NanReturnValue(ArrayWrapper::New(af::constant<unsigned>(v, dimAndType.first, dimAndType.second)));
                    return;
                case u8:
                    NanReturnValue(ArrayWrapper::New(af::constant<unsigned char>(v, dimAndType.first, dimAndType.second)));
                    return;
                case b8:
                    NanReturnValue(ArrayWrapper::New(af::constant<char>(v, dimAndType.first, dimAndType.second)));
                    return;
                default:
                    break;
            }
        }
        else if (value->IsObject())
        {
            switch (dimAndType.second)
            {
                case c32:
                    {
                        auto cv = ToFComplex(value.As<Object>());
                        NanReturnValue(ArrayWrapper::New(af::constant<af::cfloat>(cv, dimAndType.first, dimAndType.second)));
                        return;
                    }
                case c64:
                    {
                        auto cv = ToDComplex(value.As<Object>());
                        NanReturnValue(ArrayWrapper::New(af::constant<af::cdouble>(cv, dimAndType.first, dimAndType.second)));
                        return;
                    }
                default:
                    break;
            }
        }
        else if (value->IsString())
        {
            String::Utf8Value str(value);
            switch (dimAndType.second)
            {
                case s64:
                    {
                        long long val = strtoll(*str, nullptr, 10);
                        NanReturnValue(ArrayWrapper::New(af::constant<long long>(val, dimAndType.first, dimAndType.second)));
                        return;
                    }
                case u64:
                    {
                        unsigned long long val = strtoll(*str, nullptr, 10);
                        NanReturnValue(ArrayWrapper::New(af::constant<unsigned long long>(val, dimAndType.first, dimAndType.second)));
                        return;
                    }
                default:
                    break;
            }
        }
        else
        {
            FIRE_THROW("Argument at position 0 is not a constant.");
        }
        FIRE_THROW("Type is unknown.");
    }
    FIRE_CATCH
}

void InitCreateArray(v8::Handle<v8::Object> exports)
{
    exports->Set(NanNew("randu"), NanNew<FunctionTemplate>(RandU)->GetFunction());
    exports->Set(NanNew("randU"), NanNew<FunctionTemplate>(RandU)->GetFunction());
    exports->Set(NanNew("randn"), NanNew<FunctionTemplate>(RandN)->GetFunction());
    exports->Set(NanNew("randN"), NanNew<FunctionTemplate>(RandN)->GetFunction());
    exports->Set(NanNew("identity"), NanNew<FunctionTemplate>(Identity)->GetFunction());
    exports->Set(NanNew("range"), NanNew<FunctionTemplate>(Range)->GetFunction());
    exports->Set(NanNew("iota"), NanNew<FunctionTemplate>(Iota)->GetFunction());
    exports->Set(NanNew("diag"), NanNew<FunctionTemplate>(Diag)->GetFunction());
    exports->Set(NanNew("constant"), NanNew<FunctionTemplate>(Constant)->GetFunction());
}
