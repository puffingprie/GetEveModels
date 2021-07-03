import os
import requests
import concurrent.futures
import tqdm
import time

file_directory_path = os.path.dirname(os.path.realpath(__file__))
resfileindex_file_path = os.path.join(file_directory_path, "resfileindex.txt")
file_types_file_path = os.path.join(file_directory_path, "file_types.txt")
res_file_path = os.path.join(file_directory_path, "res/")


def read_File():
    appendedResFile = []
    with open(os.path.join(resfileindex_file_path, "resfileindex.txt"), "r") as file:
        resFile = file.read().split('\n')
        for i in range(len(resFile)):
            if "model/ship" in resFile[i] and ("gr2" in resFile[i] or "ar.dds" in resFile[i] or "no.dds" in resFile[i] or "pmdg.dds" in resFile[i]):
                if "lowdetail" not in resFile[i] and "mediumdetail" not in resFile[i] and "effects" not in resFile[i] and "strategiccruiser" not in resFile[i] and "wreck" not in resFile[i]:
                    appendedResFile.append(resFile[i] + '\n')

    with open(os.path.join(resfileindex_file_path, "resfileindex.txt"), "w") as file:
        for i in appendedResFile:
            file.write(i)


class file_type:
    def __init__(self, path, url):
        self.path = path
        self.url = url


def get_file_types():
    with open(resfileindex_file_path, "r") as file:
        formatted = file.read().split('\n')
        file_types = []
        for line in formatted:
            split_line = line.split("/")
            url = "https://resources.eveonline.com/" + line.split(',')[1]
            faction = split_line[4]
            ship_type = split_line[5]
            primary_identifier = split_line[6].split(",")[0]
            secondary_identifier = split_line[7].split(",")[0]
            secondary_identifier = secondary_identifier.replace(
                primary_identifier, "")
            if secondary_identifier[0] == "_":
                secondary_identifier = secondary_identifier[1:]
            tertiary_identifier = split_line[8].split(",")[0] if len(
                split_line) == 10 else ""
            file_type = ""
            if "gr2" in line:
                file_type = ".gr2"
                # primary_identifier = primary_identifier.replace(
                #     ".gr2", "")
                secondary_identifier = secondary_identifier.replace(
                    ".gr2", "")
                tertiary_identifier = tertiary_identifier.replace(
                    ".gr2", "")
            elif "ar.dds" in line:
                file_type = "_ar.dds"
                # primary_identifier = primary_identifier.replace(
                #     "_ar.dds", "")
                secondary_identifier = secondary_identifier.replace(
                    "_ar.dds", "")
                tertiary_identifier = tertiary_identifier.replace(
                    "_ar.dds", "")
            elif "no.dds" in line:
                file_type = "_no.dds"
                # primary_identifier = primary_identifier.replace(
                #     "_no.dds", "")
                secondary_identifier = secondary_identifier.replace(
                    "_no.dds", "")
                tertiary_identifier = tertiary_identifier.replace(
                    "_no.dds", "")
            elif "pmdg.dds" in line:
                file_type = "_pmdg.dds"
                # primary_identifier = primary_identifier.replace(
                #     "_pmdg.dds", "")
                secondary_identifier = secondary_identifier.replace(
                    "_pmdg.dds", "")
                tertiary_identifier = tertiary_identifier.replace(
                    "_pmdg.dds", "")
            if primary_identifier in tertiary_identifier:
                tertiary_identifier = tertiary_identifier.replace(
                    primary_identifier, "")
                tertiary_identifier = tertiary_identifier.replace(
                    secondary_identifier, "")
                tertiary_identifier = tertiary_identifier[:-1]
            is_faction_variant = False
            if tertiary_identifier != "":
                if tertiary_identifier[0] == "_":
                    tertiary_identifier = tertiary_identifier[1:]
                secondary_identifier, tertiary_identifier = tertiary_identifier, secondary_identifier
                is_faction_variant = True
            file_types.append((faction, ship_type, primary_identifier,
                              secondary_identifier, tertiary_identifier, file_type, url, is_faction_variant))

        sorted_file_types = sorted(file_types)
        # print(file_types)
        for idx, i in enumerate(sorted_file_types):
            with open(file_types_file_path, "a") as file_types_file:
                line_to_print = f"{i[0]}_{i[1]}_{i[2]}_{i[3]}"
                if i[4] == "":
                    line_to_print = f"{line_to_print}{i[5]},is_faction_variant={i[7]},{i[6]}"
                else:
                    line_to_print = f"{line_to_print}_{i[4]}{i[5]},is_faction_variant={i[7]},{i[6]}"
                # Don't write newline if idx == last
                if idx + 1 == len(sorted_file_types):
                    file_types_file.write(line_to_print)
                else:
                    file_types_file.write(line_to_print+"\n")


def download_resource(file_line):
    try:
        file_line_split = file_line.split(",")
        url = file_line_split[2]
        file_name = file_line_split[0]
        r = requests.get(url, allow_redirects=True)
        open(
            f"{res_file_path}{file_name}", "wb").write(r.content)
        # print(f"Downloaded {file_name}")
        tqdm.tqdm.write(f"Downloaded {file_name}")
    except Exception as e:
        # print("An error occured:", e)
        tqdm.tqdm.write("An error occured", e)


def download_all():
    urls = []
    paths = []
    with open(file_types_file_path, "r") as file:
        split_lines = file.read().split("\n")
        # file_lines = file.read().split("\n")
        # Gets first 10 items
        file_lines = [item for item in split_lines[0:11]]
        # for i in file_lines:
        #     split_lines = i.split(",")
        #     urls.append(split_lines[1])
        #     paths.append(split_lines[0])
    t1 = time.perf_counter()
    for i in file_lines:
        download_resource(i)
        # print(f"{round(i/len(urls) * 100, 2)}%")
    t2 = time.perf_counter()
    print(f"done in {t2-t1} second(s)")


def download_all_threads(download_type="all"):
    file_lines = []
    with open(file_types_file_path, "r") as file:
        file_lines = file.read().split("\n")
        if "all" in download_type:
            pass
        if "no_faction_variants" in download_type:
            file_lines = [
                item for item in file_lines if "is_faction_variant=False" in item]
        if "ship_models_only" in download_type:
            file_lines = [item for item in file_lines if "gr2" in item]
        # elif download_type == ""
        # file_lines = [item for item in file.read().split("\n")[0: 100]]
    t1 = time.perf_counter()
    tqdm_length = len(file_lines)
    with tqdm.tqdm(total=tqdm_length) as progress_bar:
        with concurrent.futures.ThreadPoolExecutor() as executor:
            futures = {executor.submit(
                download_resource, line): line for line in file_lines}
            results = {}
            for future in concurrent.futures.as_completed(futures):
                arg = futures[future]
                results[arg] = future.result()
                progress_bar.update()
    t2 = time.perf_counter()
    tqdm.tqdm.write(f"done in {t2-t1} second(s)")


get_file_types()
download_all()
# download_all_threads("ship_models_only")
download_all_threads()
