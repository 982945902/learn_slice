
from libcpp.string cimport string
from libcpp.vector cimport vector
from libc.stdint cimport uint16_t

from libcpp.memory cimport shared_ptr

cdef extern from "drogon/drogon.h" namespace "drogon" nogil:
    cdef enum HttpStatusCode "drogon::HttpStatusCode":
        kUnknown = 0,
        k100Continue = 100,
        k101SwitchingProtocols = 101,
        k102Processing = 102,
        k103EarlyHints = 103,
        k200OK = 200,
        k201Created = 201,
        k202Accepted = 202,
        k203NonAuthoritativeInformation = 203,
        k204NoContent = 204,
        k205ResetContent = 205,
        k206PartialContent = 206,
        k207MultiStatus = 207,
        k208AlreadyReported = 208,
        k226IMUsed = 226,
        k300MultipleChoices = 300,
        k301MovedPermanently = 301,
        k302Found = 302,
        k303SeeOther = 303,
        k304NotModified = 304,
        k305UseProxy = 305,
        k306Unused = 306,
        k307TemporaryRedirect = 307,
        k308PermanentRedirect = 308,
        k400BadRequest = 400,
        k401Unauthorized = 401,
        k402PaymentRequired = 402,
        k403Forbidden = 403,
        k404NotFound = 404,
        k405MethodNotAllowed = 405,
        k406NotAcceptable = 406,
        k407ProxyAuthenticationRequired = 407,
        k408RequestTimeout = 408,
        k409Conflict = 409,
        k410Gone = 410,
        k411LengthRequired = 411,
        k412PreconditionFailed = 412,
        k413RequestEntityTooLarge = 413,
        k414RequestURITooLarge = 414,
        k415UnsupportedMediaType = 415,
        k416RequestedRangeNotSatisfiable = 416,
        k417ExpectationFailed = 417,
        k418ImATeapot = 418,
        k421MisdirectedRequest = 421,
        k422UnprocessableEntity = 422,
        k423Locked = 423,
        k424FailedDependency = 424,
        k425TooEarly = 425,
        k426UpgradeRequired = 426,
        k428PreconditionRequired = 428,
        k429TooManyRequests = 429,
        k431RequestHeaderFieldsTooLarge = 431,
        k451UnavailableForLegalReasons = 451,
        k500InternalServerError = 500,
        k501NotImplemented = 501,
        k502BadGateway = 502,
        k503ServiceUnavailable = 503,
        k504GatewayTimeout = 504,
        k505HTTPVersionNotSupported = 505,
        k506VariantAlsoNegotiates = 506,
        k507InsufficientStorage = 507,
        k508LoopDetected = 508,
        k510NotExtended = 510,
        k511NetworkAuthenticationRequired = 511

    cdef enum HttpMethod "drogon::HttpMethod":
        Get = 0
        Post
        Head
        Put
        Delete
        Options
        Patch
        Invalid
    
    cdef enum ConstraintType "drogon::internal::ConstraintType":
        ConstraintType_None
        ConstraintType_HttpMethod
        ConstraintType_HttpMiddleware
    
    cdef cppclass HttpConstraint "drogon::internal::HttpConstraint":
        HttpConstraint(HttpMethod method)
        HttpConstraint(string middlewareName)

    cdef cppclass HttpRequest "drogon::HttpRequest":
        const string &getPath()
        const char *bodyData() const
        size_t bodyLength() const
        HttpMethod getMethod() const


    ctypedef shared_ptr[HttpRequest] HttpRequestPtr
    
    ctypedef shared_ptr[HttpResponse] HttpResponsePtr

    cdef cppclass HttpResponse "drogon::HttpResponse":
        void setStatusCode(HttpStatusCode code)
        void setBody(const string& body)
        void addHeader(string field, const string &value)

        @staticmethod
        shared_ptr[HttpResponse] newHttpResponse()

cdef inline HttpMethodToStr(HttpMethod method):
    if method == HttpMethod.Get:
        return "GET"
    elif method == HttpMethod.Post:
        return "POST"
    elif method == HttpMethod.Put:
        return "PUT"
    elif method == HttpMethod.Delete:
        return "DELETE"
    elif method == HttpMethod.Head:
        return "HEAD"
    elif method == HttpMethod.Options:
        return "OPTIONS"
    elif method == HttpMethod.Patch:
        return "PATCH"
    else:
        #return "UNKNOWN"
        raise NotImplementedError("Unknown http method")

cdef inline HttpMethod StrToHttpMethod(method):
    if method == "GET":
        return HttpMethod.Get
    elif method == "POST":
        return HttpMethod.Post
    elif method == "PUT":
        return HttpMethod.Put
    elif method == "DELETE":
        return HttpMethod.Delete
    elif method == "HEAD":
        return HttpMethod.Head
    elif method == "OPTIONS":
        return HttpMethod.Options
    elif method == "PATCH":
        return HttpMethod.Patch
    else:
        #return HttpMethod.Invalid
        raise ValueError("Invalid http method")

cdef extern from * namespace "drogon_w" nogil:
    """
    #include "drogon/HttpAppFramework.h"
    #include <thread>
    #include <iostream>

    template<typename T>
    void __Pyx_call_destructor(T& x) {
        x.~T();
    }

    namespace drogon_w {
        using namespace drogon;

        class CallBackNotify {
        public:
            CallBackNotify(std::function<void(const HttpResponsePtr&)> callback): callback_(callback) {}

            void set_done(const HttpResponsePtr& resp) {
                app().getLoop()->runInLoop([callback_=callback_, resp=resp]() {
                    callback_(resp);
                });
            }
        private:

            std::function<void(const HttpResponsePtr&)> callback_;
        };

        using CallBackNotifyPtr = std::shared_ptr<CallBackNotify>;
        
        class CallBackHandle: public DrObjectBase {
            public:
                CallBackHandle(auto handle,void* user_data):doHandle_(handle),user_data_(user_data)  {}
                virtual ~CallBackHandle() {}

                void operator()(const HttpRequestPtr& req,std::function<void(const HttpResponsePtr&)>&& callback) {
                    try {
                    auto notify = std::make_shared<CallBackNotify>(callback);
                    doHandle_(req,notify,user_data_);
                    } catch (std::exception &e) {                        
                        auto rsp = HttpResponse::newHttpResponse();
                        rsp->setStatusCode(HttpStatusCode::k500InternalServerError);
                        rsp->setBody(e.what());
                        callback(rsp);
                    }
                }
                
            private:
                void(*doHandle_)(const HttpRequestPtr&,CallBackNotifyPtr,void*);
                void* user_data_;
        };

        void registerHandler(const std::string& path,void(*func)(const HttpRequestPtr&,CallBackNotifyPtr,void*),void* user_data,const std::vector<drogon::internal::HttpConstraint>& constraints = {},const std::string& middlewareName = "") {
            drogon::app().registerHandler(
                path,
                CallBackHandle(func,user_data),
                constraints,
                middlewareName
            );
        }

        void simple_run(const std::string &ip,uint16_t port) {
            drogon::app().addListener(ip,port);
            std::thread th([&]{
            drogon::app().disableSigtermHandling().run();
            });
            th.detach();
        }
    }

    """

    cdef cppclass CallBackNotify "drogon_w::CallBackNotify":
        void set_done(const HttpResponsePtr&)

    ctypedef shared_ptr[CallBackNotify] CallBackNotifyPtr

    cdef void registerHandler(const string& path,
                            void(*)(const HttpRequestPtr &,CallBackNotifyPtr,void*),
                             void* user_data,
                             const vector[HttpConstraint]&,
                             const string&)
    
    cdef void simple_run(const string &ip, uint16_t port)