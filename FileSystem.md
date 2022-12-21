来自[这里](https://www.1point3acres.com/bbs/thread-457216-1-1.html),在笔记本摊了很久，整理一下。  

搜了一圈没找到题目，呵呵。  
至少调通能跑了，暂存一下。

```cpp

#include<iostream>
#include<memory>
#include<sstream>
#include<string>
#include<unordered_map>
#include<utility>
#include<vector>
#define DISALLOW_COPY_AND_ASSIGN(classname) \
classname(const classname & orig) = delete; \
classname & operator=(const classname & rhs) = delete
typedef std::string ValueType;
typedef std::shared_ptr<ValueType> ValueTypePtr;
class FileSystemDirectory;
inline std::string ToString(const ValueTypePtr& value) {
  if (value == nullptr) {
    return "NULL";
  }
  std::ostringstream oss;
  oss << *value;
  return oss.str();
}
// 文件系统Event类型
enum class FileSystemEventType {
  kCreate = 0,
  kSetValue
};

// 文件系统Event接口
class FileSystemEvent {
public:
  FileSystemEventType getType() const {
    return type_;
  }
  const FileSystemDirectory& getDirectory() const {
    return directory_;
  }
protected:
  FileSystemEvent(const FileSystemEventType& type,
    const FileSystemDirectory& directory)
    : type_(type),
    directory_(directory) {}
private:
  const FileSystemEventType type_;
  const FileSystemDirectory& directory_;
  DISALLOW_COPY_AND_ASSIGN(FileSystemEvent);
};
// 文件系统的Create Event
class FileSystemEventCreate : public FileSystemEvent {
public:
  explicit FileSystemEventCreate(const FileSystemDirectory& directory)
    : FileSystemEvent(FileSystemEventType::kCreate, directory) {}

private:
  DISALLOW_COPY_AND_ASSIGN(FileSystemEventCreate);
};

// 文件系统的SetValue Event
class FileSystemEventSetValue : public FileSystemEvent {
public:
  FileSystemEventSetValue(const FileSystemDirectory& directory,
    const ValueTypePtr& old_value,
    const ValueTypePtr& new_value)
    : FileSystemEvent(FileSystemEventType::kSetValue, directory),
    old_value_(old_value),
    new_value_(new_value) {}
  const ValueTypePtr& getOldValue() const {
    return old_value_;
  }
  const ValueTypePtr& getNewValue() const {
    return new_value_;
  }
private:
  const ValueTypePtr old_value_;
  const ValueTypePtr new_value_;
  DISALLOW_COPY_AND_ASSIGN(FileSystemEventSetValue);
};

// 文件系统Event Callback接口定义
class FileSystemEventCallback {
public:
  virtual ~FileSystemEventCallback() {}
  virtual void invoke(const FileSystemEvent& event) = 0;
};

// 方便in-place定义Callback的Lambda Wrapper
template < typename Callback>
class FileSystemEventLambdaCallback : public FileSystemEventCallback {
public:
  explicit FileSystemEventLambdaCallback(const Callback& callback)
    : callback_(callback) {}
  void invoke(const FileSystemEvent& event) override {
    callback_(event);
  }
private:
  const Callback callback_;
  DISALLOW_COPY_AND_ASSIGN(FileSystemEventLambdaCallback);
};

// 单个文件系统节点
class FileSystemDirectory {
public:
  FileSystemDirectory(const FileSystemDirectory* parent,
    const std::string& directory_name)
    : parent_(parent),
    directory_name_(directory_name) {}

  bool create(const std::vector<std::string>& path_components,
    const std::size_t component_index,
    const bool recursive)
  {
    if (component_index >= path_components.size()) {
      return true;
    }
    const std::string& child_name = path_components[component_index];
    const auto it = children_.find(child_name);
    FileSystemDirectory* child;
    if (it != children_.end())
    {
      child = it->second.get();
    }
    else
    {
      if (!recursive && component_index + 1 < path_components.size())
      {
        return false;
      }
      child = new FileSystemDirectory(this, child_name);
      children_.emplace(child_name, std::unique_ptr<FileSystemDirectory>(child));
      for (const auto& callback : callbacks_) {
        FileSystemEventCreate event(*child);
        callback->invoke(event);
      }
    }
    return child->create(path_components, component_index + 1, recursive);
  }

  ValueTypePtr getValue(const std::vector<std::string>& path_components,
    const std::size_t component_index) {
    if (component_index < path_components.size()) {
      const auto it = children_.find(path_components[component_index]);
      if (it == children_.end()) {
        return nullptr;
      }
      return it->second->getValue(path_components, component_index + 1);
    }
    return value_;
  }

  template < typename ValueReference>
  bool setValue(const std::vector<std::string>& path_components,
    const std::size_t component_index,
    ValueReference&& new_value) {
    if (component_index < path_components.size()) {
      const auto it = children_.find(path_components[component_index]);
      if (it == children_.end()) {
        return false;
      }
      return it->second->setValue(path_components,
        component_index + 1,
        std::forward<ValueReference>(new_value));
    }
    const ValueTypePtr old_value = value_;
    value_ = std::make_shared<ValueType>(std::forward<ValueReference>(new_value));
    for (const auto& callback : callbacks_) {
      FileSystemEventSetValue event(*this, old_value, value_);
      callback->invoke(event);
    }
    return true;
  }

  bool registerCallback(const std::vector<std::string>& path_components,
    const std::size_t component_index,
    FileSystemEventCallback* callback) {
    if (component_index < path_components.size()) {
      const auto it = children_.find(path_components[component_index]);
      if (it == children_.end()) {
        return false;
      }
      return it->second->registerCallback(path_components,
        component_index + 1,
        callback);
    }
    callbacks_.emplace_back(callback);
    return true;
  }

  const FileSystemDirectory* getParent() const {
    return parent_;
  }

  const std::string& getName() const {
    return directory_name_;
  }

  std::string getPath() const {
    std::string path;
    if (parent_ != nullptr) {
      path = parent_->getPath();
    }
    // The checking is for special handling of the root directory (i.e. '/').
    if (path.empty() || path.back() != '/') {
      path.push_back('/');
    }
    path.append(directory_name_);
    return path;
  }

private:
  const FileSystemDirectory* parent_;
  std::string directory_name_;
  ValueTypePtr value_;
  std::unordered_map<std::string, std::unique_ptr<FileSystemDirectory>> children_;
  std::vector<std::unique_ptr<FileSystemEventCallback>> callbacks_;

  DISALLOW_COPY_AND_ASSIGN(FileSystemDirectory);
};//为了补上这个我他妈的较深了那么多。

  // 文件系统的主要代码
class FileSystem {
public:
  FileSystem()
    : root_(std::make_unique<FileSystemDirectory>(nullptr, "")) {}
  // recursive为true时为递归创建，类似linux下面的mkdir -p命令
  bool create(const std::string& path, const bool recursive = false) {
    std::vector<std::string> components;
    const bool valid = SplitPathComponents(path, &components);
    if (components.empty()) {
      // Cannot re-create the root directory.
      return false;
    }
    return valid && root_->create(components,
      0 /* component_index */,
      recursive);
  }
  ValueTypePtr getValue(const std::string& path) {

    std::vector<std::string> components;

    if (!SplitPathComponents(path, &components)) {

      return nullptr;

    }

    return root_->getValue(components, 0 /* component_index */);

  }
  template < typename ValueReference>
  bool setValue(const std::string& path, ValueReference&& new_value) {
    std::vector<std::string> components;
    if (!SplitPathComponents(path, &components)) {
      return bool(nullptr);
    }
    return root_->setValue(components,
      0 /* component_index */,
      std::forward<ValueReference>(new_value));
  }
  bool registerCallback(const std::string& path,
    FileSystemEventCallback* callback) {
    std::vector<std::string> components;
    if (!SplitPathComponents(path, &components)) {
      return false;
    }
    return root_->registerCallback(components,
      0 /* component_index */,
      callback);
  }
  template < typename Callback>
  bool registerLambdaCallback(const std::string& path,
    const Callback& callback) {
    return registerCallback(
      path, new FileSystemEventLambdaCallback<Callback>(callback));
  }

private:
  static bool SplitPathComponents(const std::string& path,
    std::vector<std::string>* components) {
    // State machine.
    const int kInit = 0;
    const int kDelimiter = 1;
    const int kName = 2;
    std::string component;
    int state = 0;
    for (const char c : path) {
      switch (state) {
      case kInit:
        if (c == '/') {
          state = kDelimiter;
        }
        else if (c != ' ' && c != '\t') {
          // Invalid path expression.
          return false;
        }
        break;
      case kDelimiter:
        if (c != '/') {
          state = kName;
          component.push_back(c);
        }
        break;
      case kName:
        if (c != '/') {
          component.push_back(c);
        }
        else {
          components->emplace_back(std::move(component));
          component.clear();
          state = kDelimiter;
        }
        break;
      }
    }
    // Last component.
    if (state == kName) {
      components->emplace_back(std::move(component));
    }
    return true;
  }
  std::unique_ptr<FileSystemDirectory> root_;
  DISALLOW_COPY_AND_ASSIGN(FileSystem);
};

std::string FormatEventMessage(const FileSystemEvent& event) {
  std::ostringstream oss;
  switch (event.getType()) {
  case FileSystemEventType::kCreate: {
    const FileSystemDirectory& directory = event.getDirectory();
    oss << "A new directory with name \"" << directory.getName() << "\""
      << " has been created under path "
      << directory.getParent()->getPath();
    break;
  }
  case FileSystemEventType::kSetValue: {
    const FileSystemEventSetValue& set_value_event =
      static_cast<const FileSystemEventSetValue&>(event);
    oss << "The value for "
      << event.getDirectory().getPath()
      << " has been changed from "
      << ToString(set_value_event.getOldValue())
      << " to "
      << ToString(set_value_event.getNewValue());
    break;
  }
  default:
    break;
  }
  return oss.str();
}
int main(int argc, char* argv[]) {
  auto callback_1 = [](const FileSystemEvent& event) {
    std::cout << "Callback 1 has been triggered! The file system event is:\n"
      << "********\n* "
      << FormatEventMessage(event)
      << "\n\n";
  };
  auto callback_2 = [](const FileSystemEvent& event) {
    std::cout << "Callback 2 has been triggered! The file system event is:\n"
      << "********\n* "
      << FormatEventMessage(event)
      << "\n\n";

  };
  FileSystem fs;
  fs.create("/a");
  fs.create("/a/b");
  fs.registerLambdaCallback("/a", callback_1);
  fs.create("/a/c");
  // Output:

  //

  // Callback 1 has been triggered! The file system event is:

  // ********

  // * A new directory with name "c" has been created under path /a

  //



  fs.registerLambdaCallback("/a/c", callback_1);

  fs.registerLambdaCallback("/a/c", callback_2);



  fs.setValue("/a", "123");

  fs.setValue("/a/c", "456");

  fs.setValue("/a", "789");




  // Output:

  //

  // Callback 1 has been triggered! The file system event is:

  // ********

  // * The value for /a has been changed from NULL to 123

  //

  // Callback 1 has been triggered! The file system event is:

  // ********

  // * The value for /a/c has been changed from NULL to 456

  //

  // Callback 2 has been triggered! The file system event is:

  // ********

  // * The value for /a/c has been changed from NULL to 456

  //

  // Callback 1 has been triggered! The file system event is:

  // ********
  // * The value for /a has been changed from 123 to 789

  //
  std::cout << ToString(fs.getValue("/a")) << "\n";

  std::cout << ToString(fs.getValue("/a/b")) << "\n";

  std::cout << ToString(fs.getValue("/a/c")) << "\n";
  std::cout << ToString(fs.getValue("/a/d")) << "\n";
  // Output
  //
  // 789
  // NULL
  // 456
  // NULL
  //
}
```
