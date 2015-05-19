  %1 = alloca i32, align 4
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  %z = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  store i32 0, i32* %1
  store i32 100, i32* %x, align 4
  store i32 200, i32* %y, align 4
  store i32 300, i32* %z, align 4
  %2 = load i32* %x, align 4
  %3 = load i32* %y, align 4
  %4 = mul nsw i32 %2, %3
  ;%esp = alloca i32
  ;%tmp = mul nsw i32 %2, %3
  ;store i32 %tmp, i32* %s1

  ;%tmp = load i32* %s1
  store i32 %tmp, i32* %a, align 4
  %5 = load i32* %y, align 4
  %6 = load i32* %z, align 4
  %7 = add nsw i32 %5, %6
  store i32 %7, i32* %b, align 4
  %8 = load i32* %x, align 4
  %9 = load i32* %y, align 4
  %10 = icmp sgt i32 %8, %9
  br i1 %10, label %11, label %13

; <label>:11                                      ; preds = %0
  %12 = load i32* %x, align 4
  br label %15

; <label>:13                                      ; preds = %0
  %14 = load i32* %y, align 4
  br label %15

; <label>:15                                      ; preds = %13, %11
  %16 = phi i32 [ %12, %11 ], [ %14, %13 ]
  store i32 %16, i32* %c, align 4
  %17 = load i32* %a, align 4
  %18 = load i32* %b, align 4
  %19 = add nsw i32 %17, %18
  %20 = load i32* %c, align 4
  %21 = add nsw i32 %19, %20
  store i32 %21, i32* %d, align 4
  %22 = load i32* %d, align 4
  %23 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @.str, i32 0, i32 0), i32 %22)
  ret i32 0
