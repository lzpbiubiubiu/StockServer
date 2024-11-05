#pragma once

#define _STR(x) #x
#define __STR(x) _STR(x)

// 版本号
#define VERSION_1 1
#define VERSION_2 0
#define VERSION_3 0
#define VERSION_4 0

#define VERSION_STR __STR(VERSION_1##.##VERSION_2##.##VERSION_3##.##VERSION_4)

// 其他信息
#define COMPANY_NAME        "Dmall Inc."
#define PRODUCT_NAME        "Dmall In-store Server"
#define FILE_DESCRIPTION    "Dmall In-store Server"
#define LEGAL_COPYRIGHT     "Copyright © 2024 Dmall Inc. All rights reserved."
