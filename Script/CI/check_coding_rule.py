# coding: UTF-8
"""
python 3.7以上を要求

必要ライブラリ
"""
import pprint
import sys
import os.path
import json
import re

# How to use
# $python check_coding_rule.py check_coding_rule.json

# 環境変数
DEBUG = 0
# 0 : Release
# 1 : all
IS_SHOW_CODE_AT_ERR = 1

# コード中に現れた型
# unsigned hoge, signed hoge を除く
g_type_set = set()


def main():
    if len(sys.argv) != 2:
        print("Please give a setting file as an argumente.")
        sys.exit(1)

    setting_file_path = sys.argv[1]
    if not os.path.isfile(setting_file_path):
        print("Setting file not found.")
        sys.exit(1)

    with open(setting_file_path, encoding="utf-8", mode="r") as fh:
        settings = json.load(fh)
    if DEBUG:
        pprint.pprint(settings)

    flag = check_coding_rule(settings)

    if flag:
        print("The above files are invalid coding rule.")
        sys.exit(1)
    print("Completed!")
    sys.exit(0)


# 0: OK, 1: NG
def check_coding_rule(settings: dict) -> int:
    flag = 0

    target_dirs = []
    for target_dir in settings["target_dirs"]:
        target_dirs.append(settings["c2a_root_dir"] + target_dir)
    ignore_dirs = []
    for ignore_dir in settings["ignore_dirs"]:
        ignore_dirs.append(settings["c2a_root_dir"] + ignore_dir)
    ignore_files = []
    for ignore_file in settings["ignore_files"]:
        ignore_files.append(settings["c2a_root_dir"] + ignore_file)

    preprocess_(target_dirs, ignore_dirs, ignore_files, settings)

    for target_dir in target_dirs:
        for root, dirs, files in os.walk(target_dir):
            for file in files:
                # print(root)
                # print(file)

                if root.startswith(tuple(ignore_dirs)):
                    if DEBUG:
                        print("!!!! ignore_dirs")
                        print(root)
                        print(file)
                    continue

                ext = (os.path.splitext(file))[1]
                # print(ext)
                if ext != ".h" and ext != ".c" and ext != ".hpp" and ext != ".cpp":
                    continue
                path = root + r"/" + file
                path = path.replace("\\", "/")

                # print(path)

                if path in ignore_files:
                    if DEBUG:
                        print("!!!! ignore_files")
                        print(path)
                    continue

                ret = check_file_(path, settings)
                if ret != 0:
                    # print(path)
                    flag = 1
    return flag


def preprocess_(target_dirs: list, ignore_dirs: list, ignore_files: list, settings: dict):
    global g_type_set
    for target_dir in target_dirs:
        for root, dirs, files in os.walk(target_dir):
            for file in files:
                if root.startswith(tuple(ignore_dirs)):
                    continue
                ext = (os.path.splitext(file))[1]
                if ext != ".h" and ext != ".c" and ext != ".hpp" and ext != ".cpp":
                    continue
                path = root + r"/" + file
                path = path.replace("\\", "/")
                if path in ignore_files:
                    continue
                preprocess_inner_(path, settings)

    ignore_types = ["auto", "signed", "unsigned", "using", "typedef", "struct", "enum", "class"]
    for ignore_type in ignore_types:
        if ignore_type in g_type_set:
            g_type_set.remove(ignore_type)
    # pprint.pprint(settings['additional_type'])
    g_type_set |= set(settings["additional_type"])
    # pprint.pprint(g_type_set)


def preprocess_inner_(path: str, settings: dict):
    with open(path, encoding=settings["input_file_encoding"]) as f:
        code_lines = f.read().split("\n")

    ptn_find_type = "^ *(\w+)\*? +\w+"
    reptn_find_type = re.compile(ptn_find_type)
    ptn_replace_qualifier = (
        "^ *((const|static|extern|volatile|register)+ +)+"  # static const hoge も検出できるように
    )
    reptn_replace_qualifier = re.compile(ptn_replace_qualifier)

    # targets = ["for", "if", "while", "return", "switch", "case", "break", "else", "goto", "auto", "continue", "extern", "enum", "union", "register", "static", "struct", "typedef", "volatile"]

    control_identifier = [
        "for",
        "if",
        "while",
        "return",
        "switch",
        "case",
        "break",
        "else",
        "goto",
        "continue",
    ]
    # control_identifier = ["auto", "signed", "unsigned"]

    for idx, line in enumerate(code_lines):
        if is_in_comment_context_in_multiline_(path, code_lines, idx):
            continue
        non_qualifier_line = reptn_replace_qualifier.sub(" ", line)
        non_qualifier_line = non_qualifier_line.strip()
        if non_qualifier_line.startswith(tuple(control_identifier)):
            continue
        match = reptn_find_type.search(non_qualifier_line)
        if not match is None:
            # print(match.group(1))
            g_type_set.add(match.group(1))


# 0: OK, 1: NG
def check_file_(path: str, settings: dict) -> int:
    flag = 0

    with open(path, encoding=settings["input_file_encoding"]) as f:
        code_lines = f.read().split("\n")

    # print(path)
    # pprint.pprint(code_lines)

    if check_comment_(path, code_lines) != 0:
        flag = 1
    if check_newline_(path, code_lines) != 0:
        flag = 1
    if check_eof_(path, code_lines) != 0:
        flag = 1
    if check_space_(path, code_lines) != 0:
        flag = 1
    if check_operator_space_(path, code_lines) != 0:
        flag = 1
    if check_preprocessor_(path, code_lines) != 0:
        flag = 1
    # if check_include_guard_(path, code_lines) != 0:
    #     flag = 1

    return flag


# TODO
# 演算子の位置
# 演算子前後のスペース
# インクルードガード
# ファイル名


# 0: OK, 1: NG
def check_comment_(path: str, code_lines: list) -> int:
    flag = 0
    for idx, line in enumerate(code_lines):
        if is_in_comment_context_in_multiline_(path, code_lines, idx):
            continue
        if "//" in line:
            if not has_started_with_list_after_target_(line, "//", [" ", "!< "]):
                print(path + ": " + str(idx + 1) + ": SPACE IS REQUIRED AFTER '//' OR '//!<'")
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1
        if "/*" in line:
            if not has_started_with_list_after_target_(
                line, "/*", [" ", "!< ", "*"]
            ):  # TODO: "*" を許すの．．．
                print(path + ": " + str(idx + 1) + ": SPACE IS REQUIRED AFTER '/*' OR '/*!<'")
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1
        if "*/" in line:
            if not has_started_with_list_after_target_(line, "*/", [" "]):
                print(path + ": " + str(idx + 1) + ": SPACE IS REQUIRED AFTER '*/'")
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1

        if "//" in line:
            if not has_ended_with_list_before_target_(line, "//", [" "]):
                print(path + ": " + str(idx + 1) + ": SPACE IS REQUIRED BEFORE '//'")
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1
        if "/*" in line:
            if not has_ended_with_list_before_target_(line, "/*", [" "]):
                print(path + ": " + str(idx + 1) + ": SPACE IS REQUIRED BEFORE '/*'")
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1
        if "*/" in line:
            if not has_ended_with_list_before_target_(line, "*/", [" ", "*"]):  # TODO: "*" を許すの．．．
                print(path + ": " + str(idx + 1) + ": SPACE IS REQUIRED BEFORE '*/'")
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1

    return flag


# 0: OK, 1: NG
def check_newline_(path: str, code_lines: list) -> int:
    flag = 0
    counter = 0
    for idx, line in enumerate(code_lines):
        if line == "":
            counter += 1
        else:
            counter = 0
        if counter >= 4:
            print(path + ": " + str(idx + 1) + ": TOO MANY CONSECUTIVE LINE BREAKS")
            if IS_SHOW_CODE_AT_ERR:
                print(line)
            flag = 1

    for idx, line in enumerate(code_lines):
        if is_in_comment_context_in_multiline_(path, code_lines, idx):
            continue

        if line.find("for") == -1:  # TODO: for があると文途中に ; があるので．今後治す
            if has_line_ended_with_target_(line, ";") == 0:
                print(
                    path
                    + ": "
                    + str(idx + 1)
                    + ": THE END OF A STATEMENT SHOULD BE A COMMENT OR A LINE BREAK"
                )
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1

        if has_line_ended_with_target_(line, "{") == 0:
            non_comment_line = remove_comment_and_strip_(line)
            if non_comment_line[-1] in [
                ",",
                "+",
                "-",
                "*",
                "/",
                "%",
                "&",
                "|",
                "^",
                "~",
            ]:  # 複数行で { 111, 111,
                pass
            elif non_comment_line[-2:] == "};":  # uint8_t command_id[3] = {0x76, 0x61, 0x73};
                pass
            else:
                print(path + ": " + str(idx + 1) + ": ALLMAN STYLE IS REQUIRED")
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1
        # 旧版
        # # if not (line.find("[") != -1 and line.find("};") != -1):      # TODO: uint8_t command_id[3] = {0x76, 0x61, 0x73}; などを含めないため．今後治す
        # # if line.find("};") == -1:                                   # TODO: uint8_t command_id[3] = {0x76, 0x61, 0x73}; などを含めないため．今後治す
        # if line.find("[") == -1 and line.find("};") == -1:        # TODO: uint8_t command_id[3] = {0x76, 0x61, 0x73}; などを含めないため．今後治す
        #     if (has_line_ended_with_target_(line, "{") == 0):
        #         print(path + ": " + str(idx + 1) + ": ALLMAN STYLE IS REQUIRED")
        #         if IS_SHOW_CODE_AT_ERR:
        #             print(line)
        #         flag = 1

        # # targets = ["class", "enum", "struct", "else"]
        # targets = [" class ", " enum ", " struct "]
        # for target in targets:
        #     if (has_line_ended_with_target_(line, target) == 0):
        #         print(path + ": " + str(idx + 1) + ": ALLMAN STYLE IS REQUIRED")
        #         if IS_SHOW_CODE_AT_ERR:
        #             print(line)
        #         flag = 1

        # TODO: だいぶ雑
        non_comment_line1 = line.split("//", 1)[0]
        non_comment_line2 = line.split("/*", 1)[0]
        if min(len(non_comment_line1), len(non_comment_line2)) > 150:
            if line[0:2] == "//" or line[0:2] == "/*":
                pass
            elif line[0:7] == "#define":  # TODO: これは改行すると怖いので許す？
                pass
            else:
                print(path + ": " + str(idx + 1) + ": ONE LINE (EXCLUDING COMMENTS) IS TOO LONG")
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1

    targets = ["class", "enum", "struct", "if", "for", "else", "while", "switch", "case"]
    for target in targets:
        all_text = "".join(code_lines)
        if all_text.find(target) == -1:
            continue

        ptn = "^(|.*(\W))(" + re.escape(target) + ")(\W).*\{.*(" + re.escape("//|/*") + ")?"
        reptn = re.compile(ptn)
        for idx, line in enumerate(code_lines):
            if is_in_comment_context_in_multiline_(path, code_lines, idx):
                continue

            match = reptn.search(line)
            if not match is None:
                print(path + ": " + str(idx + 1) + ": ALLMAN STYLE IS REQUIRED")
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1

    targets = ["else"]
    for target in targets:
        all_text = "".join(code_lines)
        if all_text.find(target) == -1:
            continue

        ptn = "^.*\}.*(\W)(" + re.escape(target) + ")(\W.*|)$"
        reptn = re.compile(ptn)
        for idx, line in enumerate(code_lines):
            if is_in_comment_context_in_multiline_(path, code_lines, idx):
                continue

            match = reptn.search(line)
            if not match is None:
                print(path + ": " + str(idx + 1) + ": ALLMAN STYLE IS REQUIRED")
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1

    return flag


# 0: OK, 1: NG
def check_eof_(path: str, code_lines: list) -> int:
    max_line = len(code_lines)
    if max_line == 0:
        return 0
    if max_line == 1:
        if code_lines[-1] == "":
            return 0
        else:
            print(path + ": 1: NO NEW LINE AT EOF")
            if IS_SHOW_CODE_AT_ERR:
                print(code_lines[-1])
            return 1
    if code_lines[-1] != "":
        print(path + ": " + str(max_line) + ": NO NEW LINE AT EOF")
        if IS_SHOW_CODE_AT_ERR:
            print(code_lines[-1])
        return 1
    if code_lines[-1] == "" and code_lines[-2] == "":
        print(path + ": " + str(max_line - 2) + ": TOO MANY LINE BREAKS AT EOF")
        if IS_SHOW_CODE_AT_ERR:
            print(code_lines[-2])
        return 1
    return 0


# 0: OK, 1: NG
def check_space_(path: str, code_lines: list) -> int:
    flag = 0
    for idx, line in enumerate(code_lines):
        if "\t" in line:
            print(path + ": " + str(idx + 1) + ": TAB IS NOT PERMITTED")
            if IS_SHOW_CODE_AT_ERR:
                print(line)
            flag = 1
        if "　" in line:
            print(path + ": " + str(idx + 1) + ": ZENKAKU SPACE IS NOT PERMITTED")
            if IS_SHOW_CODE_AT_ERR:
                print(line)
            flag = 1
        if len(line) != 0:
            if line[-1] == " ":
                print(path + ": " + str(idx + 1) + ": ANY SPACE AT EOL NEEDS TO BE REMOVED")
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1

    return flag


# 0: OK, 1: NG
def check_operator_space_(path: str, code_lines: list) -> int:
    flag = 0

    for idx, line in enumerate(code_lines):
        if is_in_comment_context_in_multiline_(path, code_lines, idx):
            continue
        if "," in line:
            if not has_started_with_list_after_target_(line, ",", [" "]):
                print(path + ": " + str(idx + 1) + ": SPACE IS REQUIRED AFTER ','")
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1

        if ";" in line:
            if not has_started_with_list_after_target_(line, ";", [" "]):
                print(path + ": " + str(idx + 1) + ": SPACE IS REQUIRED AFTER ';'")
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1

        # ↓hoge_point * 10 などが int * で引っかかった
        # targets = ["uint8_t *", "uint16_t *", "uint32_t *",
        #            "int8_t *", "int16_t *", "int32_t *", "size_t *",
        #            "_t *",
        #            "void *", "int *", "short *", "char *", "long *", "float *", "double *"]
        # for target in targets:
        #     if is_contained_pattern_(line, target):
        #         print(path + ": " + str(idx + 1) + ": PROHIBITED PATTERNS:'" + target + "'")
        #         if IS_SHOW_CODE_AT_ERR:
        #             print(line)
        #         flag = 1

        # if ")" in line:
        #     if not has_started_with_list_after_target_(line, ")", [" ", ")", ";"]):
        #         print(path + ": " + str(idx + 1) + ": SPACE IS REQUIRED AFTER ')'")
        #         if IS_SHOW_CODE_AT_ERR:
        #             print(line)
        #         flag = 1
        # if "(" in line:
        #     if not has_ended_with_list_before_target_(line, "(", [" "]):
        #         print(path + ": " + str(idx + 1) + ": SPACE IS REQUIRED BEFORE '('")
        #         if IS_SHOW_CODE_AT_ERR:
        #             print(line)
        #         flag = 1
        if "}" in line:
            if not has_started_with_list_after_target_(line, "}", [" ", ";", ","]):
                print(path + ": " + str(idx + 1) + ": SPACE IS REQUIRED AFTER '}'")
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1
        # if "{" in line:
        #     if not has_ended_with_list_before_target_(line, "{", [" ", "{"]):
        #         print(path + ": " + str(idx + 1) + ": ALLMAN STYLE IS REQUIRED OR SPACE IS REQUIRED BEFORE '{'")
        #         if IS_SHOW_CODE_AT_ERR:
        #             print(line)
        #         flag = 1

        targets = ["}{", "){", "}("]
        for target in targets:
            if is_contained_pattern_(line, target):
                print(
                    path
                    + ": "
                    + str(idx + 1)
                    + ": PROHIBITED PATTERNS:'"
                    + target
                    + "' (SPACE IS REQUIRED)"
                )
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1

        targets = ["for", "if", "while", "switch", "case", "else", "do"]
        strip_line = line.strip()
        for target in targets:
            if strip_line.startswith(target) and len(strip_line) > len(target):
                next_char = strip_line[len(target)]
                if next_char in ["(", "{"]:
                    print(path + ": " + str(idx + 1) + ": SPACE IS REQUIRED AFTER '" + target + "'")
                    if IS_SHOW_CODE_AT_ERR:
                        print(line)
                    flag = 1

    targets = [x + " *" for x in g_type_set]
    for target in targets:
        all_text = "".join(code_lines)
        if all_text.find(target) == -1:
            continue

        ptn = "(\W)" + "(" + re.escape(target) + ")"
        reptn = re.compile(ptn)
        for idx, line in enumerate(code_lines):
            if is_in_comment_context_in_multiline_(path, code_lines, idx):
                continue

            poss = [x.start() for x in reptn.finditer(line)]
            for pos in poss:
                if not is_in_comment_context_in_line_(line, pos):
                    print(
                        path
                        + ": "
                        + str(idx + 1)
                        + ": '*' MUST BE PLACED TO THE SIDE OF TYPE AT '"
                        + target
                        + "'"
                    )
                    if IS_SHOW_CODE_AT_ERR:
                        print(line)
                    flag = 1
    targets = [x + " &" for x in g_type_set]
    for target in targets:
        all_text = "".join(code_lines)
        if all_text.find(target) == -1:
            continue

        ptn = "(\W)" + "(" + re.escape(target) + ")"
        reptn = re.compile(ptn)
        for idx, line in enumerate(code_lines):
            if is_in_comment_context_in_multiline_(path, code_lines, idx):
                continue

            poss = [x.start() for x in reptn.finditer(line)]
            for pos in poss:
                if not is_in_comment_context_in_line_(line, pos):
                    print(
                        path
                        + ": "
                        + str(idx + 1)
                        + ": '&' MUST BE PLACED TO THE SIDE OF TYPE AT '"
                        + target
                        + "'"
                    )
                    if IS_SHOW_CODE_AT_ERR:
                        print(line)
                    flag = 1

    # これは endif で ifとかがヒットするのでNG
    # # targets = ["for", "if", "while", "return", "switch", "case", "break", "else", "goto", "auto", "continue", "extern", "enum", "union", "register", "static", "struct", "typedef", "volatile"]
    # targets = ["for", "if", "while", "switch", "case", "else", "goto", "auto", "extern", "enum", "union", "register", "static", "struct", "typedef", "volatile"]
    # for idx, line in enumerate(code_lines):
    #     if is_in_comment_context_in_multiline_(path, code_lines, idx):
    #         continue
    #     for target in targets:
    #         if not is_there_space_befor_after_(line, target):
    #             print(path + ": " + str(idx + 1) + ": SPACE IS REQUIRED BEFORE AND AFTER '" + target + "'")
    #             if IS_SHOW_CODE_AT_ERR:
    #                 print(line)
    #             flag = 1

    targets = ["<", ">", "=", "&", "|", "^", "~", "=", "?", ":", "!", "+", "-", "*", "/", "%"]
    for target in targets:
        ptn_before = (
            "(\w+)("
            + "["
            + re.escape("<>=&|^~=?:!+-*/&")
            + "]*"
            + re.escape(target)
            + "["
            + re.escape("<>=&|^~=?:!+-*/&")
            + "]*)(.*)"
        )
        reptn_before = re.compile(ptn_before)
        ptn_after = (
            "("
            + "["
            + re.escape("<>=&|^~=?:!+-*/&")
            + "]*"
            + re.escape(target)
            + "["
            + re.escape("<>=&|^~=?:!+-*/&")
            + "]*)(\w+)"
        )
        reptn_after = re.compile(ptn_after)

        ptn_after_monadic_operator = "(\w+)( *)(" + re.escape(target) + ")(\w+)"
        reptn_after_monadic_operator = re.compile(ptn_after_monadic_operator)

        for idx, line in enumerate(code_lines):
            if is_in_comment_context_in_multiline_(path, code_lines, idx):
                continue

            matches = reptn_before.finditer(line)
            for match in matches:
                if is_in_comment_context_in_line_(line, match.start()):
                    continue
                if is_in_string_context_(line, match.start()):
                    continue
                if match.group(2) in ["->", "++", "--", "::", "::~"]:
                    continue
                if match.group(2) == ":" and line.find("case", 0, match.start()):  # case hoge:
                    continue
                if (
                    match.group(2) in [">", "/"] and line[0:8] == "#include"
                ):  # #include <src_core/TlmCmd/command_dispatcher.h> など
                    continue
                if match.group(2) in ["-", "+"]:  # 10.5e-10 -> 5e, - でひっかかる
                    ptn = "\d+(e|E)"
                    reptn = re.compile(ptn)
                    if not reptn.search(match.group(1)) is None:
                        continue
                if match.group(2) in ["*", "&"] and match.group(1) in g_type_set:
                    continue
                if match.group(2) == "*" and match.group(3) != "":
                    if match.group(3)[0] == ")":
                        continue
                        # common_tlm_cmd_packet_util.h の `(*((type*)( \` が引っかからないように
                # print(line)
                # print(match)
                # print("#" + match.group(1) + "#")
                # print("#" + match.group(2) + "#")

                print(
                    path
                    + ": "
                    + str(idx + 1)
                    + ": SPACE IS REQUIRED BEFORE AND AFTER BINARY OPERATOR"
                )
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1

            matches = reptn_after.finditer(line)
            for match in matches:
                if is_in_comment_context_in_line_(line, match.start()):
                    continue
                if is_in_string_context_(line, match.start()):
                    continue
                if match.group(1) in [
                    "->",
                    "++",
                    "--",
                    "::",
                    "::~",
                    "&",
                    "!",
                ]:  # TOOD: "&" は二項演算子のときに漏れが発生する・・・
                    continue
                if (
                    match.group(1) in ["<", "/"] and line[0:8] == "#include"
                ):  # #include <src_core/TlmCmd/command_dispatcher.h> など
                    continue
                if match.group(1) in ["+", "-", "*", "~"]:  # TODO 単項演算子問題
                    before_line = line[: match.start()].strip()
                    if before_line == "":
                        continue
                    if before_line[-6:] == "return":
                        continue
                    ptn = "[\w\]\}]$"
                    reptn = re.compile(ptn)
                    # print(line)
                    # print(reptn.search(before_line))
                    if reptn.search(before_line) is None:
                        continue
                    if match.group(1) == "*":
                        ptn = "\w+$"
                        reptn = re.compile(ptn)
                        m = reptn.search(before_line).group()
                        if m == "else" or m in g_type_set:
                            continue
                    if match.group(1) in ["-", "+"]:
                        ptn = "\d+(e|E)$"
                        reptn = re.compile(ptn)
                        if not reptn.search(before_line) is None:
                            continue

                # print(line)
                # print(match)
                # print("#" + match.group(1) + "#")
                # print("#" + match.group(2) + "#")

                print(
                    path
                    + ": "
                    + str(idx + 1)
                    + ": SPACE IS REQUIRED BEFORE AND AFTER BINARY OPERATOR"
                )
                if IS_SHOW_CODE_AT_ERR:
                    print(line)
                flag = 1

    return flag


# 0: OK, 1: NG
def check_preprocessor_(path: str, code_lines: list) -> int:
    flag = 0
    for idx, line in enumerate(code_lines):
        if "#" in line:
            pos = line.find("#")

            if pos == 0:
                continue

            if is_in_non_string_code(path, code_lines, idx, pos):
                continue

            print(
                path + ": " + str(idx + 1) + ": PREPROCESSOR DIRECTIVES DO NOT REQUIRE INDENTATION"
            )
            if IS_SHOW_CODE_AT_ERR:
                print(line)
            flag = 1

    return flag


# TODO: すべてのファイルがキャメルケースになったら追加する
# 0: OK, 1: NG
# def check_include_guard_(path: str, code_lines: list) -> int:
#     basename, ext = os.path.splitext(os.path.basename(path))
#     if not (ext == ".h" or ext == ".hpp"):
#         return 0
#     # print(path)
#     # print(basename)
#     # print(ext)
#
#     # 最初に発見したプリプロセッサディレクティブがインクルードガードであること，
#     # さらにそれが適切な識別子であることを判断
#
#     for idx, line in enumerate(code_lines):
#         if line.startswith("#"):
#             # 初めて "#" を検出したものがインクルードガードでないとNG
#             if idx + 1 >= len(code_lines):
#                 return 1
#
#             line_ifndef = line
#             line_define = code_lines[idx + 1]
#             # print(line_ifndef)
#             # print(line_define)
#
#             if not (line_ifndef.startswith("#ifndef ") and line_define.startswith("#define ")):
#                 return 1
#
#             include_guard = basename.upper() + "_H_"
#             return 0
#
#         else:
#             continue
#
#     return 1


# 1: target が含まれる, 0: なし
def is_contained_pattern_(line: str, target: str) -> int:
    pos = 0
    find_begin = 0
    target_len = len(target)
    while 1:
        pos = line.find(target, find_begin)
        if pos == -1:  # そもそも存在しない
            return 0
        if is_in_string_context_(line, pos):
            pass
        elif is_in_comment_context_in_line_(line, pos):
            pass
        else:
            return 1
        find_begin = pos + target_len

    return 0


# 1: target の前後にスペースあり, 0: なし
def is_there_space_befor_after_(line: str, target: str) -> int:
    if target in line:
        if not has_started_with_list_after_target_(line, target, [" "]):
            return 0
    if target in line:
        if not has_ended_with_list_before_target_(line, target, [" "]):
            return 0
    return 1


# 1: target の後に指定文字列 (list) あり or 行末, 0: なし
def has_started_with_list_after_target_(line: str, target: str, starts: list) -> int:
    pos = 0
    find_begin = 0
    target_len = len(target)
    while 1:
        pos = line.find(target, find_begin)
        if pos == -1:  # そもそも存在しない
            return 1
        if pos + target_len == len(line):  # 行末
            return 1
        if is_in_string_context_(line, pos):
            pass
        elif is_in_comment_context_in_line_(line, pos):
            pass
        elif not line[pos + target_len :].startswith(tuple(starts)):
            return 0
        find_begin = pos + target_len

    return 1


# 1: target の前に指定文字列 (list) あり or 行頭, 0: なし
def has_ended_with_list_before_target_(line: str, target: str, ends: list) -> int:
    pos = 0
    find_end = len(line)
    target_len = len(target)
    while 1:
        pos = line.rfind(target, 0, find_end)
        if pos == -1:  # そもそも存在しない
            return 1
        if pos == 0:  # 行頭
            return 1
        if is_in_string_context_(line, pos):
            pass
        elif is_in_comment_context_in_line_(line, pos):
            pass
        elif not line[:pos].endswith(tuple(ends)):
            return 0
        find_end = pos

    return 1


# 1: target で終わるか，それが含まれていない, 0: その他
def has_line_ended_with_target_(line: str, target: str) -> int:
    pos_target = 0
    find_begin = 0
    target_len = len(target)
    while 1:
        # 文字列の中にある場合はOKとする
        # その場合，２番目移行も探索する
        # コメントコンテキストの場合は飛ばす
        pos_target = line.find(target, find_begin)
        if pos_target == -1:  # そもそも存在しない
            return 1
        if pos_target + target_len == len(line):  # 行末
            return 1
        if is_in_string_context_(line, pos_target):
            find_begin = pos_target + target_len
            continue
        else:
            if is_in_comment_context_in_line_(line, pos_target):
                return 1  # TODO: コーナーケースは残ってるが．．．
            break

    for pos in range(pos_target + 1, len(line)):  # そのあとコメントであればOK
        if line[pos] == " ":
            continue
        elif line[pos] == "/":
            if (pos + 1) < len(line):
                if line[pos + 1] == "/" or line[pos + 1] == "*":
                    return 1
            return 0
        else:
            return 0

    return 0


# 1: コードの中, 0: 外
def is_in_code(path: str, lines: list, line_no: int, pos: int) -> int:
    if is_in_comment_context_in_multiline_(path, lines, line_no):
        return 0
    if is_in_comment_context_in_line_(lines[line_no], pos):
        return 0
    return 1


# 1: 文字列以外のコードの中, 0: 外
def is_in_non_string_code(path: str, lines: list, line_no: int, pos: int) -> int:
    if is_in_code(path, lines, line_no, pos):
        return 1
    if is_in_string_context_(lines[line_no], pos):
        return 0
    return 1


# 1: コメントの中, 0: 外
# 単一行用
def is_in_comment_context_in_line_(line: str, pos: int) -> int:
    before = line[:pos]  # pos より先の文字列
    after = ""  # pos より後の文字列
    if len(line) > pos:
        after = line[pos + 1 :]

    find_begin = 0
    while 1:
        pos = before.find("//", find_begin)
        if pos == -1:
            break
        else:
            if is_in_string_context_(before, pos):
                find_begin = pos + 1
                continue
            else:
                return 1

    find_begin = 0
    while 1:
        pos = before.find("/*", find_begin)
        if pos == -1:
            break
        else:
            if is_in_string_context_(before, pos):
                find_begin = pos + 1
                continue
            else:
                return 1
        # TODO: 本当は "*/" も考慮に入れないとだめだが，一旦なしで

    return 0


# 1: コメントの中, 0: 外
# コメントブロック用
# TODO: 色々雑
def is_in_comment_context_in_multiline_(path: str, lines: list, line_no: int) -> int:
    if path in is_in_comment_context_in_multiline_.memo:
        if str(line_no) in is_in_comment_context_in_multiline_.memo[path]:
            return is_in_comment_context_in_multiline_.memo[path][str(line_no)]
        else:
            is_in_comment_context_in_multiline_.memo[path][str(line_no)] = 0
    else:
        is_in_comment_context_in_multiline_.memo[path] = {}
        is_in_comment_context_in_multiline_.memo[path][str(line_no)] = 0

    count_start = 0
    count_end = 0
    for i in range(line_no):
        count_start += lines[i].count("/*")
        count_end += lines[i].count("*/")

    if count_start > count_end:
        is_in_comment_context_in_multiline_.memo[path][str(line_no)] = 1
        return 1
    return 0


is_in_comment_context_in_multiline_.memo = {}  # メモ最適化のためのstatic変数


# 1: 文字列リテラルの中, 0: 外
def is_in_string_context_(line: str, pos: int) -> int:
    before = line[:pos]  # pos より先の文字列

    num_of_quotation = before.count('"') - before.count('\\"')
    if num_of_quotation % 2 == 1:
        return 1

    num_of_quotation = before.count("'") - before.count("\\'")
    if num_of_quotation % 2 == 1:
        return 1

    return 0


def remove_comment_and_strip_(line: str) -> str:
    line = remove_comment_and_strip_.reptn1.sub(" ", line)
    line = remove_comment_and_strip_.reptn2.sub(" ", line)
    return line.strip()


remove_comment_and_strip_.pnt1 = re.escape("/*") + ".*" + re.escape("*/")
remove_comment_and_strip_.reptn1 = re.compile(remove_comment_and_strip_.pnt1)
remove_comment_and_strip_.pnt2 = re.escape("//") + ".*$"
remove_comment_and_strip_.reptn2 = re.compile(remove_comment_and_strip_.pnt2)


if __name__ == "__main__":
    main()
