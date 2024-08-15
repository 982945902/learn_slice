from libcpp.vector cimport vector
from libcpp.string cimport string
from libc.stdint cimport uint16_t
from cpython cimport Py_INCREF, Py_DECREF

cimport drogon_py

import asyncio
import logging
import inspect
import uvloop

class App:
    support_methods = ["GET", "POST", "HEAD", "PUT", "DELETE", "OPTIONS", "PATCH"]
    def __init__(self):
        self.dynamic_py_user_func = {}
        self.loop = uvloop.new_event_loop()
        asyncio.set_event_loop(self.loop)

        for method in App.support_methods:
            def __create_method(method):
                def __reg_method(path,ser_pack='text'):
                    def __reg(func):
                        self.dynamic_py_user_func[f"{method} {path}"] = PyFuncSchema(method,func,ser_pack)
                        constraints = vector[drogon_py.HttpConstraint]()
                        constraints.push_back(drogon_py.HttpConstraint(drogon_py.StrToHttpMethod(method)))
                        logging.info(f"register {method} {path}")
                        drogon_py.registerHandler(path.encode('utf-8'),
                                    <void(*)(const drogon_py.HttpRequestPtr &,drogon_py.CallBackNotifyPtr,void*) noexcept nogil>handleRequest,
                                    <void*>self.dynamic_py_user_func,
                                    constraints,
                                    "")
                    return __reg
                return __reg_method
            setattr(self,method,__create_method(method))

    def run(self,ip,port):
        self.dynamic_py_user_func["this"] = self
        cdef string ip_ = ip.encode('utf-8')
        port = <uint16_t> port
        _run(ip_,port)

        self.loop.run_forever()

cdef void _run(string ip,uint16_t port) noexcept nogil:
    drogon_py.simple_run(ip,port)

class PyFuncSchema:
    def __init__(self,method,func,ser_pack):
        self._method = method
        self._func = func
        self.is_coroutine = inspect.iscoroutinefunction(func)

        if ser_pack == 'text':
            def marshal(body):
                if isinstance(body,str):
                    return body.encode('utf-8')
                return body
            def unmarshal(data):
                return data
            setattr(self,'marshal',marshal) 
            setattr(self,'unmarshal',unmarshal)

cdef void handleRequest(const drogon_py.HttpRequestPtr& req,drogon_py.CallBackNotifyPtr noty,void* user_data) noexcept nogil:
    rsp = drogon_py.HttpResponse.newHttpResponse()

    with gil:
        try:
            dynamic_py_user_func = <dict> user_data            
            method = drogon_py.HttpMethodToStr(req.get().getMethod())            
            path = req.get().getPath()
            schema = dynamic_py_user_func[f"{method} {path.decode('utf-8')}"]
            input = schema.unmarshal(req.get().bodyData())
            app = dynamic_py_user_func['this']

            if schema.is_coroutine:
                async def task():
                    try:
                        py_ret = await schema._func(input)
                        body = schema.marshal(py_ret)
                        rsp.get().setBody(body)
                        noty.get().set_done(rsp)
                    except Exception as e:
                        rsp.get().setStatusCode(drogon_py.HttpStatusCode.k500InternalServerError)
                        rsp.get().setBody(e.what())
                        noty.get().set_done(rsp)

                _ = asyncio.run_coroutine_threadsafe(task(),app.loop)
            else:
                py_ret = schema._func(input)
                body = schema.marshal(py_ret)
                rsp.get().setBody(body)
                noty.get().set_done(rsp)

        except Exception as e:
            rsp.get().setStatusCode(drogon_py.HttpStatusCode.k500InternalServerError)
            rsp.get().setBody(e.what())
            noty.get().set_done(rsp)
            
    return