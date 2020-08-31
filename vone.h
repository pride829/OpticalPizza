/*
目的：vone是virtual optical network embedding的縮寫，基本上所有處理都會在vone中執行。但是它僅僅是一個呼叫方法的介面，它裡面包含的
程式碼應該越少越好(但或許少不到哪去)。
*/
#pragma once

double vone(int CAP_OR_LRC, int FR_OR_PATH_FIRST, double alpha);