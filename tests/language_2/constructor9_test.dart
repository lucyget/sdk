// Copyright (c) 2011, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

// Check that all final instance fields of a class are initialized by
// constructors.

class Klass {
  Klass(var v) : field_ = v {}
  final uninitializedFinalField_; // //# 01: compile-time error
  var field_;
}

main() {
  new Klass(5);
}
