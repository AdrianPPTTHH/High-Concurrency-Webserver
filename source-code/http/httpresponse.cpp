#include "./httpresponse.h"


const std::unordered_map<std::string, std::string> HttpResponse::SUFFIX_TYPE{
    {".html", "text/html"},
    {".xml", "text/xml"},
    { ".xhtml", "application/xhtml+xml" },
    { ".txt",   "text/plain" },
    { ".rtf",   "application/rtf" },
    { ".pdf",   "application/pdf" },
    { ".word",  "application/nsword" },
    { ".png",   "image/png" },
    { ".gif",   "image/gif" },
    { ".jpg",   "image/jpeg" },
    { ".jpeg",  "image/jpeg" },
    { ".au",    "audio/basic" },
    { ".mpeg",  "video/mpeg" },
    { ".mpg",   "video/mpeg" },
    { ".avi",   "video/x-msvideo" },
    { ".gz",    "application/x-gzip" },
    { ".tar",   "application/x-tar" },
    { ".css",   "text/css "},
    { ".js",    "text/javascript "},
};


const std::unordered_map<int, std::string> HttpResponse::CODE_STATUS{
    {200, "OK"},
    {400, "Bad Request"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {302, "Temporarily Moved"},
};


const std::unordered_map<int, std::string> HttpResponse::CODE_PATH{
    { 400, "/400.html" },
    { 403, "/403.html" },
    { 404, "/404.html" },
};


HttpResponse::HttpResponse(){
    code_ = -1;
    path_ = srcDir_ = "";
    isKeepAlive_ = false;
    mmFile_ = nullptr;
    mmFileStat_ = {0};
}


HttpResponse::~HttpResponse(){
    UnmapFile();
}


void HttpResponse::Init(const std::string& srcDir, std::string& path, 
        bool isKeepAlive, int code){
    assert(srcDir != "");
    if(mmFile_){ UnmapFile(); }
    srcDir_ = srcDir;
    path_ = path;
    code_ = code;
    isKeepAlive_ = isKeepAlive;
    mmFile_ = nullptr;
    mmFileStat_ = {0};
}

// 将对象获取的内容 写入buff （在httpconn中将&buff写入fd中）
void HttpResponse::MakeResponse(Buffer & buff){

    // stat()获取文件元数据  成功返回0
    // S_ISDIR()用于获取stat结构中的st_mode 文件类型定位 是否是目录
    if(stat((srcDir_ + path_).data(), &mmFileStat_) < 0 || S_ISDIR(mmFileStat_.st_mode)){
        code_ = 404;
        ErrorHtml_();
    }
    else if(!(mmFileStat_.st_mode & S_IROTH)){ //查看是否对文件有读权限
        code_ = 403;
    }
    else if(code_ == -1){
        code_ = 200;
    }
    
    ErrorHtml_();
    AddStateLine_(buff);
    AddHeader_(buff);
    AddContent_(buff);
}

// 返回文件内容 映射到 内存的指针
char * HttpResponse::File(){
    return mmFile_;
}


size_t HttpResponse::FileLen() const{
    return mmFileStat_.st_size;
}


void HttpResponse::ErrorHtml_(){
    if(CODE_PATH.count(code_) == 1){
        // path_ = "/404.html"
        path_ = CODE_PATH.find(code_)->second;
        stat((srcDir_ + path_).data(), &mmFileStat_);
    }
}


void HttpResponse::AddStateLine_(Buffer& buff){
    std::string status;
    if(CODE_STATUS.count(code_) == 1){
        status = CODE_STATUS.find(code_)->second;
    }else{
        code_ = 400;
        status = CODE_STATUS.find(code_)->second;
    }
    buff.Append("HTTP/1.1 " + std::to_string(code_) + " " + status + "\r\n");
}


void HttpResponse::AddHeader_(Buffer & buff){
    buff.Append("Connection:");
    if(isKeepAlive_){
        buff.Append("keep-alive\r\n");
        buff.Append("keep-alive: max=6 , timeout=120\r\n");
    }else{
        buff.Append("close\r\n");
    }
    
    buff.Append("Content-type:" + GetFileType_() + "\r\n");
}


void HttpResponse::AddContent_(Buffer & buff){
    int srcFd = open((srcDir_ + path_ ).data(), O_RDONLY);
    if(srcFd < 0){
        ErrorContent(buff, "File NotFound!");
        return;
    }

    LOG_DEBUG("file path %s", (srcDir_ + path_).data());

    // 映射到内核的地址(null则自动分配）、要映射内存区域大小、需要保护的标志、映射对象类型、文件描述符、文件偏移量
    // 返回的是被映射区域的虚拟地址
    int* mmRet = (int*)mmap(0, mmFileStat_.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);

    if(*mmRet == -1){
        ErrorContent(buff, "File NotFound!");
        return;
    }

    // 强制转换mmRet指向内容的格式， 从指向int转换成指向char类型，即字符串指针
    mmFile_ = (char*)mmRet;
    
    buff.Append("Content-length: " + std::to_string(mmFileStat_.st_size) + "\r\n\r\n");
    close(srcFd);
    // buff.Append(mmFile_, mmFileStat_.st_size);
}


void HttpResponse::ErrorContent(Buffer& buff, std::string message){
    std::string body,status;

    body += "<html><title>Error</title>";
    body += "<body bgcolor=\"ffffff\">";
    if(CODE_STATUS.count(code_) == 1){
        status = CODE_STATUS.find(code_) -> second;
    }else{
        status = "Bad Request";
    }

    body += std::to_string(code_) + ":" + status + "\n";
    body += "<p>" + message + "</p>";
    body += "<hr><em>MyWebServer</em></body></html>";
    
    buff.Append("Content-length:" + std::to_string(body.size()) + "\r\n\r\n");
    buff.Append(body);
    
}

std::string HttpResponse::GetFileType_(){
    if(path_ == ""){
        return "";
    }
    
    size_t index = path_.find_last_of('.');

    if(index == std::string::npos){
        return "text/plain";
    }
    else{
        std::string suffix = path_.substr(index);
        if(SUFFIX_TYPE.find(suffix) != SUFFIX_TYPE.end()){
            return SUFFIX_TYPE.find(suffix)->second;
        }
    }
    
    return "text/plain";
}


void HttpResponse::UnmapFile(){
    if(mmFile_){
        munmap(mmFile_, mmFileStat_.st_size);
        mmFile_ = nullptr;
    }
}