
import re
import copy
import click

class Tools:
    @staticmethod
    def replace_comma(input_str):
        new_str = ""
        for idx,c in enumerate(input_str):
            if c != ',':
                new_str += c
            else:
                if idx>0 and idx<len(input_str)-1 and input_str[idx-1:idx+2]=="\',\'":
                    new_str += c
                else:
                    new_str += ', '
        return new_str

    @staticmethod
    def replace_special(input_str, src_pattern="\\\"", target_pattren="@$"):
        # 替换特殊模式的子字符串
        copy_str = [c for c in input_str]
        non_space_count = -1
        pos = []
        if len(input_str) <= 1:
            return pos, input_str
        for idx in range(len(input_str) - 1):
            if input_str[idx] != ' ':
                non_space_count += 1
                if input_str[idx:idx + 2] == src_pattern:
                    pos.append(non_space_count)
                    for count_, c_char in enumerate(target_pattren):
                        copy_str[idx + count_] = c_char
        return pos, ''.join(copy_str)

    @staticmethod
    def mark_double_quotation(input_str):
        # 解析双引号的位置，并替换为特殊连续无空格字符，最后还原回来即可
        copy_str = [c for c in input_str]
        non_space_count = -1
        pos = []
        pos_with_space = []
        raw_str = []
        if len(input_str) <= 1:
            return input_str, pos, raw_str
        for idx in range(len(input_str)):
            if input_str[idx] != ' ':
                non_space_count += 1
                if input_str[idx] == '"':
                    pos.append(non_space_count)
                    pos_with_space.append(idx)

        if (len(pos) % 2) != 0:
            raise ValueError("双引号不配对，请检查。{}".format(input_str))
        for idx in range(0, len(pos_with_space), 2):
            raw_str.append((input_str[pos_with_space[idx]:pos_with_space[idx + 1] + 1]))
            for pos_j in range(pos_with_space[idx], pos_with_space[idx + 1] + 1):
                if copy_str[pos_j] != ' ':
                    copy_str[pos_j] = '#'
        return ''.join(copy_str), pos, raw_str

    @staticmethod
    def inv_mark_double_quotation(input_str, pos, raw_str):
        if len(pos) == 0:
            return input_str

        new_str = ""
        last_end_pos = 0
        count = -1
        for pos_idx in range(0, len(pos), 2):
            count += 1
            left_pos = Tools.get_pos_with_space(pos[pos_idx], input_str)
            right_pos = Tools.get_pos_with_space(pos[pos_idx + 1], input_str)

            new_str += input_str[last_end_pos:left_pos]
            new_str += raw_str[count]
            last_end_pos = right_pos + 1
        new_str += input_str[last_end_pos:]
        return new_str

    @staticmethod
    def get_pos_with_space(pos, input_str):
        non_space_count = -1
        for idx in range(len(input_str)):
            if input_str[idx] != ' ':
                non_space_count += 1
                if non_space_count == pos:
                    return idx
        else:
            raise ValueError("位置错误。")

    @staticmethod
    def remove_trailing_spaces(input_str):
        # 去掉左侧空格
        skip = True
        output_str = ""
        for c in input_str:
            if c != ' ' and skip == True:
                skip = False
            if skip == False:
                output_str += c
        return output_str

    @staticmethod
    def form_indent(size):
        indent = ""
        indent += " " * size
        return indent

    @staticmethod
    def count_bracket(input_str):
        left_bracket = 0
        right_bracket = 0
        for c in input_str:
            if c == '{':
                left_bracket = left_bracket + 1
            if c == '}':
                right_bracket = right_bracket + 1
        return left_bracket, right_bracket

    @staticmethod
    def count_small_bracket(input_str):
        left_bracket = 0
        right_bracket = 0
        for c in input_str:
            if c == '(':
                left_bracket = left_bracket + 1
            if c == ')':
                right_bracket = right_bracket + 1
        return left_bracket, right_bracket

    @staticmethod
    def count_left_space(input_str):
        # 计算左侧空格数
        beg_idx = -1
        for i in range(0, len(input_str)):
            if input_str[i] != ' ':
                beg_idx = i
                break
        return beg_idx

    @staticmethod
    def count_some_char(input_str, target_char):
        counter = 0
        pos = []
        for idx, c in enumerate(input_str):
            if c == target_char:
                counter = counter + 1
                pos.append(idx)
        return counter, pos

    @staticmethod
    def insert_some_char(input_str, pos, target_char):
        list_input_str = list(input_str)
        list_input_str.insert(pos, target_char)
        input_str = ''.join(list_input_str)
        return input_str

    @staticmethod
    def replace_tab(input_str, space_count = 4):
        new_str = ""
        for c in input_str:
            if c != '\t':
                new_str += c
            else:
                new_str += ''*4
        return new_str

    @staticmethod
    def separate_left_space(input_str):
        if input_str[0] != ' ':
            return "", input_str
        else:
            for idx, c in enumerate(input_str):
                if c != ' ':
                    return input_str[:idx], input_str[idx:]

class Cfg:
    def __init__(self):
        self.cfg = {
            "indent_size": 4,
            "space_in_arguments_definition": "no",
            "space_in_condition": "one",
            "block_statement": "one"}

    def get(self, key):
        return self.cfg[key]

    def Total_Replace(self, Aline, numLine, raw_line_str, formater):
        tmp_space_store = ""
        if formater.last_line_unmatched_small_bracket != 0 :
            tmp_space_store, Aline = Tools.separate_left_space(raw_line_str)

        _, res_str = Tools.replace_special(Aline)
        new_Aline, a2, a3 = Tools.mark_double_quotation(res_str)

        Aline = Tools.replace_tab(new_Aline, 4)
        Aline = self.Rule_1(Aline)
        Aline = self.Rule_2(Aline)
        Aline = self.Rule_3(Aline)
        Aline = self.Rule_4(Aline)
        Aline = self.Rule_5(Aline)
        Aline = self.Rule_6(Aline)
        Aline = self.Rule_7(Aline)

        inv_str = Tools.inv_mark_double_quotation(Aline, a2, a3)
        _, res_str = Tools.replace_special(inv_str, "@$", "\\\"")
        if formater.last_line_unmatched_small_bracket != 0 :
            res_str = tmp_space_store + res_str.strip(' ')
        return res_str

    def Rule_1(self, Aline):
        # 除了开头的空格外，删除其余地方多余的空格间隙，只保留一个空格间隙
        beg_idx = Tools.count_left_space(Aline)
        new_Aline = Tools.form_indent(beg_idx)
        next_space_valid = True
        for i in range(beg_idx, len(Aline)):
            if Aline[i] != ' ':
                new_Aline += Aline[i]
                next_space_valid = True
            elif next_space_valid == True:
                new_Aline += ' '
                next_space_valid = False
        return new_Aline

    def Rule_2(self, Aline):
        # 一些简单规则
        Aline = Aline.replace("){\n", ") {\n")
        Aline = Aline.replace("if(", "if (")
        Aline = Aline.replace("while(", "while (")
        Aline = Aline.replace("switch(", "switch (")
        Aline = Aline.replace("for(", "for (")
        Aline = Aline.replace("else{", "else {")
        Aline = Aline.replace(" )", ")")
        return Aline

    def Rule_3(self, Aline):
        # 对于紧凑的间距，适当在其周围插入一个space
        # 对于';', 若其不在最开始位置，也不在最后位置，则';x'之间添加一个空位
        new_Aline = ""
        for i in range(0, len(Aline)):
            if Aline[i] != ';':
                new_Aline += Aline[i]
            else:
                if i != len(Aline) - 1 and (Aline[i + 1] != ' ' and Aline[i + 1] != '\n'):
                    new_Aline += '; '
                else:
                    new_Aline += ';'
        return new_Aline

    def Rule_4(self, Aline):

        Aline = Tools.replace_comma(Aline)

        Aline = Aline.replace("+=", " += ")
        Aline = Aline.replace("-=", " -= ")
        Aline = Aline.replace("*=", " *= ")
        Aline = Aline.replace("/=", " /= ")
        Aline = Aline.replace("%=", " %= ")
        Aline = Aline.replace("==", " == ")
        Aline = Aline.replace("!=", " != ")

        Aline = Aline.replace("<<=", " <<= ")
        Aline = Aline.replace(">>=", " >>= ")
        Aline = Aline.replace("&=", " &= ")
        Aline = Aline.replace("^=", " ^= ")
        Aline = Aline.replace("|=", " |= ")
        Aline = Aline.replace(">=", " >= ")
        Aline = Aline.replace("<=", " <= ")
        Aline = Aline.replace("<<", " << ")
        Aline = Aline.replace(">>", " >> ")

        Aline = Aline.replace("&&", " && ")
        Aline = Aline.replace("||", " || ")

        Aline = self.Rule_1(Aline)  # 可能会引入多个空格，再缩减一下
        return Aline

    def Rule_5(self, Aline):
        # 实现 = 而排出 += -= 等的自动周围空格填充
        forbidens  = set(["+=", "-=","*=", "/=", "%=", "==", "!=", "<=", ">=", "&=", "^=", "|="])
        new_str = ""
        for idx, c in enumerate(Aline):
            if c != '=':
                new_str += c
            else:
                if idx<len(Aline)-1 and Aline[idx:idx+2] == "==":
                    new_str += c
                elif Aline[idx-1:idx+1] in forbidens:
                    new_str += c
                else:
                    new_str += " = "
        Aline = self.Rule_1(new_str)  # 可能会引入多个空格，再缩减一下
        return Aline

    def Rule_6(self, Aline):
        # 实现 < 而排出 <= 等的自动周围空格填充
        if not ("if (" in Aline or "while (" in Aline or "for (" in Aline or "switch (" in Aline):
            return Aline

        forbidens = set(["<=", "<<"])
        new_str = ""
        for idx, c in enumerate(Aline):
            if c != '<':
                new_str += c
            else:
                if idx < len(Aline) - 1 and Aline[idx:idx + 2] in forbidens:
                    new_str += c
                elif idx>0 and Aline[idx-1:idx+1] in forbidens:
                    new_str += c
                else:
                    new_str += " < "
        Aline = self.Rule_1(new_str)  # 可能会引入多个空格，再缩减一下
        return Aline

    def Rule_7(self, Aline):
        # 实现 < 而排出 <= 等的自动周围空格填充
        if not ("if (" in Aline or "while (" in Aline or "for (" in Aline or "switch (" in Aline):
            return Aline

        forbidens = set([">=", ">>","->"])
        new_str = ""
        for idx, c in enumerate(Aline):
            if c != '>':
                new_str += c
            else:
                if idx < len(Aline) - 1 and Aline[idx:idx + 2] in forbidens:
                    new_str += c
                elif idx > 0 and Aline[idx - 1:idx + 1] in forbidens:
                    new_str += c
                else:
                    new_str += " > "
        Aline = self.Rule_1(new_str)  # 可能会引入多个空格，再缩减一下
        return Aline

class Formater:
    def __init__(self, inFile):
        self.code_file = inFile
        self.strings = []
        self.margins = []
        self.unmatched_small_bracket = 0
        self.last_line_unmatched_small_bracket = 0

        self.LoadFile()
        self.Process()
        self.SaveFile()

    def LoadFile(self):
        f1 = open(self.code_file, 'r', encoding='UTF-8')
        line_num = 1
        for Aline in f1.readlines():
            # 去掉\n 前的空格
            if Aline[-1] != '\n':
                Aline = Aline + '\n'
                print("文件{}在第{}行的末端不是换行符, 已补上换行符.".format(self.code_file, line_num))
            if len(Aline) > 1:
                Aline = Aline[:-1].rstrip(' ') + '\n'
            self.strings.append(Aline)
            line_num += 1

    def SaveFile(self):
        tmps = self.code_file.split('.')
        save_file = ""
        for i in range(len(tmps) - 1):
            if tmps[i] == "":
                save_file = save_file + "."
            else:
                save_file = save_file + tmps[i] + '.'
        save_file = save_file[:-1] + '_out.' + tmps[-1]
        f1 = open(save_file, 'w', encoding='UTF-8')
        f1.write(self.page)

    def Process(self):
        self.Align()

    def Insert_Space(self, Aline, numLine, raw_line_str):
        '''插入必需的字符串'''
        Aline = self.cfg.Total_Replace(Aline, numLine, raw_line_str,self)
        return Aline

    def Align(self):
        self.cfg = Cfg()

        margin = 0
        page = ""
        self.broken = False
        number = 1

        for line_str in self.strings:  # 依次处理每行文本代码
            raw_line_str = copy.copy(line_str)
            self.last_line_unmatched_small_bracket = self.unmatched_small_bracket
            # 统计当前行是否只包含一个左括号
            left_small_bracket_num, right_small_bracket_num = Tools.count_small_bracket(raw_line_str)
            owing_small_bracket_num = left_small_bracket_num - right_small_bracket_num
            if owing_small_bracket_num != 0:
                self.unmatched_small_bracket += owing_small_bracket_num

            # Case 1： 包含左括号
            if re.match(r'.*{.*', line_str) and not self.broken:
                left_barcket, right_bracket = Tools.count_bracket(line_str)
                line_str = Tools.remove_trailing_spaces(line_str)
                if self.last_line_unmatched_small_bracket == 0:
                    for n in range(0, margin):
                        page += Tools.form_indent(self.cfg.get("indent_size"))
                self.margins.append(margin)
                if left_barcket != right_bracket:
                    margin += 1
            elif re.match(r'.*}.*', line_str) and not self.broken:
                if margin > 0:
                    margin -= 1
                else:
                    self.broken = True
                    self.broken_line_number = number
                line_str = Tools.remove_trailing_spaces(line_str)
                if self.last_line_unmatched_small_bracket == 0:
                    for n in range(0, margin):
                        page += Tools.form_indent(self.cfg.get("indent_size"))
                self.margins.append(margin)
            elif not self.broken:
                line_str = Tools.remove_trailing_spaces(line_str)
                if self.last_line_unmatched_small_bracket == 0:
                    for n in range(0, margin):
                        page += Tools.form_indent(self.cfg.get("indent_size"))
                self.margins.append(margin)

            # 准备写入当前行
            line_str = self.Insert_Space(line_str, number, raw_line_str)
            page += line_str
            number += 1

        self.page = page


@click.command()
@click.option("-f", help='The name of c++ code file')
def main(f):
    # TODO Python命令行模式  支持单个文件  或者 指定目录下的所有.h .hpp .cpp .cc的文件
    file = f #"./demo1.cpp"
    formater = Formater(file)


if __name__ == "__main__":
    main()
