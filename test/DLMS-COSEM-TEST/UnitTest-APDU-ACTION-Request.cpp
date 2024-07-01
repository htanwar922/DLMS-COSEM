// ===========================================================================
// Copyright (c) 2018, Electric Power Research Institute (EPRI)
// All rights reserved.
//
// DLMS-COSEM ("this software") is licensed under BSD 3-Clause license.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// *  Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// *  Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// *  Neither the name of EPRI nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
// OF SUCH DAMAGE.
//
// This EPRI software incorporates work covered by the following copyright and permission
// notices. You may not use these works except in compliance with their respective
// licenses, which are provided below.
//
// These works are provided by the copyright holders and contributors "as is" and any express or
// implied warranties, including, but not limited to, the implied warranties of merchantability
// and fitness for a particular purpose are disclaimed.
//
// This software relies on the following libraries and licenses:
//
// ###########################################################################
// Boost Software License, Version 1.0
// ###########################################################################
//
// * asio v1.10.8 (https://sourceforge.net/projects/asio/files/)
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// 

#include <gtest/gtest.h>

#include "APDU/ACTION-Request.h"

using namespace EPRI;

//<action-request-normal>
//  <invoke-id-and-priority>
//    <invoke-id>1</invoke-id>
//    <is-priority>True</is-priority>
//    <requires-confirmation>False</requires-confirmation>
//  </invoke-id-and-priority>
//  <cosem-method-descriptor>
//    <class-id>9</class-id>
//    <instance-id raw="00 00 0a 00 00 ff">0-0:10.0.0.255</instance-id>
//    <method-id>1</method-id>
//  </cosem-method-descriptor>
//  <method-invocation-parameters>
//    <long-unsigned>2</long-unsigned>
//  </method-invocation-parameters>
//</action-request-normal>
static const std::vector<uint8_t> FINAL = 
{ 
    0xC3, 0x01, 0x81, 0x00, 0x09, 0x00, 0x00, 0x0A, 
    0x00, 0x00, 0xFF, 0x01, 0x01, 0x12, 0x00, 0x02
};

//<action-request-normal>
//  <invoke-id-and-priority>
//    <invoke-id>1</invoke-id>
//    <is-priority>True</is-priority>
//    <requires-confirmation>False</requires-confirmation>
//  </invoke-id-and-priority>
//  <cosem-method-descriptor>
//    <class-id>9</class-id>
//    <instance-id raw="00 00 0a 00 00 ff">0-0:10.0.0.255</instance-id>
//    <method-id>1</method-id>
//  </cosem-method-descriptor>
//</action-request-normal>
static const std::vector<uint8_t> FINAL1 = 
{ 
    0xC3, 0x01, 0x81, 0x09, 0x09, 0x01, 0x10, 0x0A, 
    0x42, 0x34, 0xFF, 0x01, 0x00
};

TEST(ACTION_Request, GeneralUsage) 
{
    Action_Request_Normal Request;
    DLMSVector            Data(FINAL);
    
    ASSERT_TRUE(Request.Parse(&Data, 1, 1));
    ASSERT_EQ(0x81, Request.invoke_id_and_priority);
    ASSERT_EQ(0x0009, Request.cosem_method_descriptor.class_id);
    ASSERT_EQ(DLMSVector({0x00, 0x00, 0x0A, 0x00, 0x00, 0xFF}), Request.cosem_method_descriptor.instance_id);
    ASSERT_EQ(0x01, Request.cosem_method_descriptor.method_id);
    ASSERT_TRUE((bool)Request.method_invocation_parameters);
    ASSERT_EQ(DLMSVector({ 0x01, 0x12, 0x00, 0x02 }),
              Request.method_invocation_parameters.value());
    
    ASSERT_EQ(FINAL, Request.GetBytes());
    Request.cosem_method_descriptor.method_id = 4;
    ASSERT_EQ(
        std::vector<uint8_t>({ 0xC3, 0x01, 0x81, 0x00, 0x09, 0x00, 0x00, 0x0A, 
                               0x00, 0x00, 0xFF, 0x04, 0x01, 0x12, 0x00, 0x02 }),
        Request.GetBytes());
    

    DLMSVector            Data1(FINAL1);
    Request.Clear();
    ASSERT_TRUE(Request.Parse(&Data1, 1, 1));
    ASSERT_EQ(0x81, Request.invoke_id_and_priority);
    ASSERT_EQ(0x0909, Request.cosem_method_descriptor.class_id);
    ASSERT_EQ(DLMSVector({ 0x01, 0x10, 0x0A, 0x42, 0x34, 0xFF }), Request.cosem_method_descriptor.instance_id);
    ASSERT_EQ(0x01, Request.cosem_method_descriptor.method_id);
    ASSERT_FALSE((bool)Request.method_invocation_parameters);
    
    ASSERT_EQ(FINAL1, Request.GetBytes());
    
}