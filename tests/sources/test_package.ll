; ModuleID = 'test_package'
source_filename = "test_package"
target triple = "x86_64-pc-linux-gnu"

@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @init_globals, ptr null }]
@a_glb = global i16 15, align 2
@b_glb = global i8 1, align 1
@d = global i32 0, align 4
@"foo::s" = internal global i8 4, align 1

define i32 @init_d() #0 {
allocation:
  br label %entry

entry:                                            ; preds = %allocation
  %0 = load i32, ptr @d, align 4
  ret i32 %0
}

define i32 @bar() #0 {
allocation:
  br label %entry

entry:                                            ; preds = %allocation
  %0 = load i16, ptr @a_glb, align 2
  %upcasted = sext i16 %0 to i32
  %1 = mul i32 %upcasted, 100
  %2 = load i8, ptr @b_glb, align 1
  %upcasted1 = sext i8 %2 to i32
  %3 = mul i32 %upcasted1, 20
  %4 = call i16 @foo()
  %upcasted2 = sext i16 %4 to i32
  %5 = mul i32 %3, %upcasted2
  %6 = sub i32 %1, %5
  ret i32 %6
}

define i32 @foobar(i16 %0, i8 %1) #0 {
allocation:
  br label %entry

entry:                                            ; preds = %allocation
  %upcasted = sext i8 %1 to i16
  %2 = mul i16 %0, %upcasted
  %upcasted1 = sext i16 %2 to i32
  ret i32 %upcasted1
}

; Function Attrs: mustprogress noinline norecurse nounwind optnone
define i32 @main() #1 {
allocation:
  %0 = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 0, ptr %0, align 4
  br label %entry

entry:                                            ; preds = %allocation
  %1 = call i16 @foo()
  %2 = call i32 @bar()
  %3 = call i32 @foobar(i16 2, i8 3)
  %4 = add i32 %2, %3
  store i32 %4, ptr %a, align 4
  store i8 -1, ptr @b_glb, align 1
  %5 = load i32, ptr %a, align 4
  %6 = load i16, ptr @a_glb, align 2
  %7 = load i8, ptr @b_glb, align 1
  %8 = add i8 %7, 10
  %9 = call i32 @foobar(i16 %6, i8 %8)
  %10 = add i32 %5, %9
  store i32 %10, ptr %a, align 4
  %11 = load i32, ptr %a, align 4
  ret i32 %11
}

define i16 @foo() #0 {
allocation:
  %result = alloca i16, align 2
  br label %entry

entry:                                            ; preds = %allocation
  %0 = load i8, ptr @"foo::s", align 1
  %upcasted = sext i8 %0 to i16
  %1 = sdiv i16 12, %upcasted
  %2 = load i8, ptr @b_glb, align 1
  %upcasted1 = sext i8 %2 to i16
  %3 = add i16 %1, %upcasted1
  store i16 %3, ptr %result, align 2
  %4 = load i8, ptr @"foo::s", align 1
  %5 = sub i8 %4, 1
  store i8 %5, ptr @"foo::s", align 1
  %6 = load i16, ptr %result, align 2
  ret i16 %6
}

define private void @init_globals() section ".text.startup" {
d_init:
  %0 = call i32 @init_d()
  store i32 %0, ptr @d, align 4
  br label %exit

exit:                                             ; preds = %d_init
  ret void
}

attributes #0 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #1 = { mustprogress noinline norecurse nounwind optnone "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
