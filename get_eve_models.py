import os
import subprocess
import requests
import concurrent.futures
import tqdm
import time
from pathlib import Path

file_directory_path = os.path.dirname(os.path.realpath(__file__))
resfileindex_file_path = os.path.join(file_directory_path, "resfileindex.txt")
file_types_file_path = os.path.join(file_directory_path, "file_types.txt")
res_file_path = os.path.join(file_directory_path, "res/")
obj_models_file_path = os.path.join(file_directory_path, "obj_models/")

# Create /res folder if not exist
Path(res_file_path).mkdir(parents=True, exist_ok=True)
# Create /obj_models folder if not exist
Path(obj_models_file_path).mkdir(parents=True, exist_ok=True)


def read_file():
    appendedResFile = []
    with open(resfileindex_file_path, "r") as file:
        resFile = file.read().split('\n')
        for i in range(len(resFile)):
            if "model/ship" in resFile[i] and ("gr2" in resFile[i] or "ar.dds" in resFile[i] or "no.dds" in resFile[i] or "pmdg.dds" in resFile[i]):
                if "lowdetail" not in resFile[i] and "mediumdetail" not in resFile[i] and "effect" not in resFile[i] and "strategiccruiser" not in resFile[i] and "wreck" not in resFile[i]:
                    appendedResFile.append(resFile[i])

    with open(resfileindex_file_path, "w") as file:
        for idx, i in enumerate(appendedResFile):
            if(idx == len(appendedResFile) - 1):
                file.write(i)
            else:
                file.write(i + "\n")


def make_associated_res_file():
    with open(os.path.join(file_directory_path, "associated_file_names.txt"), "r") as associated_file_names_file:
        associated_file_names_list = associated_file_names_file.read().split("\n")
        associated_file_names_dict = dict((identifier, ship_name) for identifier, ship_name in (
            i.rsplit(": ") for i in associated_file_names_list))
    with open(resfileindex_file_path, "r") as res_file:
        res_file_list = res_file.read().split("\n")
        for idx, line in enumerate(res_file_list):
            line_fields_list = line.split(",")[0].split("/")
            # print(line)
            url = "https://resources.eveonline.com/" + line.split(",")[1]
            faction = line_fields_list[4]
            ship_type = line_fields_list[5]
            file_info_list = line_fields_list[-1].split("_")
            faction_variant = ""
            if len(file_info_list) == 4:
                identifier = f"{file_info_list[0]}_{file_info_list[1]}"
                faction_variant = file_info_list[2]
                file_type = f"_{file_info_list[3]}"
            elif len(file_info_list) == 2:
                temp = file_info_list[1].split(".")
                identifier = f"{file_info_list[0]}_{temp[0]}"
                file_type = ".gr2"
            elif len(file_info_list) == 3:
                identifier = f"{file_info_list[0]}_{file_info_list[1]}"
                if ".gr2" in file_info_list[2]:
                    temp = file_info_list[2].split(".")
                    faction_variant = temp[0]
                    file_type = ".gr2"
                else:
                    file_type = f"_{file_info_list[2]}"
            else:
                identifier = file_info_list[0]
                file_type = f"_{file_info_list[2]}"
            if identifier in associated_file_names_dict:
                # Change identifier to ship name
                identifier = associated_file_names_dict[identifier].replace(
                    " ", "-")
            if faction_variant != "":
                res_file_list[idx] = f"{faction}_{ship_type}_{identifier}_{faction}{file_type},is_faction_variant=True,{url}"
            else:
                res_file_list[idx] = f"{faction}_{ship_type}_{identifier}{file_type},is_faction_variant=False,{url}"
    with open(file_types_file_path, "w") as res_file:
        for idx, line in enumerate(res_file_list):
            if idx == len(res_file_list) - 1:
                res_file.write(line)
            else:
                res_file.write(line + "\n")


def download_resource(file_line):
    try:
        file_line_split = file_line.split(",")
        url = file_line_split[2]
        file_name = file_line_split[0]
        r = requests.get(url, allow_redirects=True)
        open(
            f"{res_file_path}{file_name}", "wb").write(r.content)
        # print(f"Downloaded {file_name}")
        # tqdm.tqdm.write(f"Downloaded {file_name}")
        return f"Downloaded {file_name}"
    except Exception as e:
        tqdm.tqdm.write("An error occured: ", e)
        return


# def download_all():
#     urls = []
#     paths = []
#     with open(file_types_file_path, "r") as file:
#         split_lines = file.read().split("\n")
#         # file_lines = file.read().split("\n")
#         # Gets first 10 items
#         file_lines = [item for item in split_lines[0:11]]
#         # for i in file_lines:
#         #     split_lines = i.split(",")
#         #     urls.append(split_lines[1])
#         #     paths.append(split_lines[0])
#     t1 = time.perf_counter()
#     for i in file_lines:
#         download_resource(i)
#         # print(f"{round(i/len(urls) * 100, 2)}%")
#     t2 = time.perf_counter()
#     print(f"Done in {t2-t1} second(s)")


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
        if "graphic_files_only" in download_type:
            file_lines = [
                item for item in file_lines if "ar.dds" in item or "no.dds" in item or "pmdg.dds" in item]
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
                progress_bar.set_description(results[arg])
                progress_bar.update()
    t2 = time.perf_counter()
    tqdm.tqdm.write(f"Done in {t2-t1} second(s)")


def convert_gr2(gr2file):
    gr2converttool_file_path = os.path.join(
        file_directory_path, "gr2convert/evegr2toobj.exe")
    gr2file_file_path = os.path.join(res_file_path, gr2file)
    objfile_file_path = os.path.join(
        obj_models_file_path, gr2file.replace(".gr2", ".obj"))
    subprocess.run([gr2converttool_file_path,
                   gr2file_file_path, objfile_file_path])


def convert_all_gr2():
    # Get all .gr2 files
    files = os.listdir(res_file_path)
    gr2_files_list = [file for file in files if file.endswith(".gr2")]
    t1 = time.perf_counter()
    with concurrent.futures.ThreadPoolExecutor() as executor:
        executor.map(convert_gr2, gr2_files_list)
    t2 = time.perf_counter()
    print(f"Converted in {t2-t1} second(s)")


read_file()
make_associated_res_file()
# download_all() #use download_all_threads() instead
# download_all_threads("ship_models_only")
download_all_threads("graphic_files_only")
# download_all_threads()
convert_all_gr2()
