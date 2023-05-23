#pragma once

/// <summary>
/// A generic class for returning the result of an operation.
/// 
/// The operation may succeed or fail. In the event of a failure, this class provides failure information.
/// </summary>
/// <typeparam name="T">The type of the 'result' object.</typeparam>
template<class T> class Result {
public:

  static Result<T> Ok(const T& result) { return Result("", result); }
  static Result<T> Error(const T& error) { return Result(error, ""); }

  const std::string error;

  /// <returns>true if the operation succeeded and 'result()' may be called.</returns>
  bool ok() const { return error.empty(); }

  /// <summary>
  /// Retrieve the result value of the operation.
  /// 
  /// It's an error to call this method when 'ok()' returns false. The result is undefined in that case.
  /// </summary>
  const T& result() const {
    assert(ok());
    return _result;
  }

private:
  Result(const T& _error, const T& result) :
    _result(result),
    error(_error)
  {}

  const T _result;
};


