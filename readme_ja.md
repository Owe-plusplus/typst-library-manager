# typst-library-manager

[English README](readme.md)

`tylm` は、Git リポジトリで管理している Typst ライブラリをローカルに取得し、そのライブラリを import する新しい Typst 文書／プロジェクトを作成するコマンドラインツールです。

## 必要なもの

- C++20 に対応したコンパイラ
- CMake 3.15 以降
- `git`（ライブラリの取得時に使用）
- Typst（生成した文書をコンパイルする場合）

## ビルドとインストール

```sh
cmake -S . -B build
cmake --build build
cmake --install build --prefix ~/.local
```

`~/.local/bin` が `PATH` に含まれていない環境では、追加してから `tylm` を実行してください。

インストールせずに試す場合は、生成された実行ファイルを直接使えます。

```sh
./build/tylm --help
```

## ダウンロード

ビルド済みの Linux 向けパッケージは [GitHub Releases](https://github.com/Owe-plusplus/typst-library-manager/releases) から入手できます。使用するディストリビューション向けのパッケージをダウンロードすれば、CMake や C++ コンパイラは不要です。

- `.deb`: Debian / Ubuntu 系ディストリビューション向け
- `.tar.gz`: それ以外の Linux ディストリビューション向け

## 使い方

### 0. ライブラリを作成する

ライブラリは Git リポジトリとして用意します。リポジトリ直下には、基本的にライブラリ名と同じ名前の `.typ` または `.typst` ファイルを置いてください。たとえば `my-library` という名前で登録する場合の構成は次のとおりです。

```text
my-library/
└── my-library.typ
```

`add` が生成する文書には `#import "…/my-library.typ" : *` と `#show : setup` が含まれます。そのため、エントリーファイルでは文書全体に適用する初期設定を行う `setup` 関数を公開します。

```typst
// my-library.typ
#let setup(body) = {
  set page(paper: "a4", margin: 20mm)
  set text(font: "Noto Sans CJK JP", size: 10pt)
  body
}
```

この例では、ライブラリを使う文書のページ設定と文字設定が自動で適用されます。章スタイルなど、ほかの共通設定も `setup` 内に追加できます。

ライブラリ名と一致するファイルがない場合でも、リポジトリ直下の最初の `.typ`／`.typst` ファイルは利用できます。ただし、どのファイルが選ばれるかを明確にするため、ライブラリ名と同じエントリーファイルを置くことを推奨します。

### 1. ライブラリを登録する

登録情報は `~/.typst_library_manager/config.json` に保存されます。ファイルや親ディレクトリが存在しない場合は、`tylm` の実行時に空の設定ファイル（`[]`）として自動生成されます。`name` はローカルのディレクトリ名と、ライブラリ内で優先的に探すエントリーファイル名に使われます。

```sh
tylm config add \
  --name my-library \
  --url https://github.com/example/my-library.git
```

設定ファイルを直接編集して登録することもできます。`~/.typst_library_manager/config.json` を作成または編集し、次のような JSON 配列を記述してください。

```json
[
  {
    "name": "my-library",
    "url": "https://github.com/example/my-library.git"
  }
]
```

複数のライブラリを登録する場合は、同じ形式のオブジェクトを配列に追加します。`name` は重複させないでください。通常は重複チェックを行う `config add` の利用を推奨します。

登録を削除するには、次を実行します。

```sh
tylm config remove --name my-library
```

### 2. 作業ディレクトリでライブラリを取得する

Typst プロジェクトを置きたいディレクトリで実行してください。登録済みの各リポジトリが、カレントディレクトリ配下の `libraries/<name>/` に clone されます。

```sh
cd path/to/your-typst-workspace
tylm init
```

`init` は、取得したリポジトリの直下に `.typ` または `.typst` ファイルがあることを確認します。見つからない場合はエラーで停止します。意図的にそのようなリポジトリを保持したい場合は `--force` を付けて、検証をスキップしてください。

```sh
tylm init --force
```

### 3. プロジェクトを作る

以下は `report/main.typ` を作成します。`--library` を省略すると、初期化済みライブラリの先頭が使われます。

```sh
tylm add report --docname main --library my-library
```

生成されるファイルには、選択したライブラリのソースファイルへの相対 import と `#show : setup` が書き込まれます。

```typst
#import "../libraries/my-library/my-library.typ" : *
#show : setup
```

`--doc` を指定すると、プロジェクト用ディレクトリを作らず、カレントディレクトリに単一文書を作成します。

```sh
tylm add handout --doc --library my-library
```

この例では `handout.typ` が作られます。単一文書モードでは `--docname` は使用されません。

### オプションの短縮形

短縮形と長い形式は同じ意味です。

| コマンド                       | 短縮形    | 長い形式         | 内容                                       |
| ------------------------------ | --------- | ---------------- | ------------------------------------------ |
| `init`                         | `-f`      | `--force`        | ソースファイルの検証をスキップして続行する |
| `add`                          | `-n TEXT` | `--docname TEXT` | プロジェクト内に作る文書名を指定する       |
| `add`                          | `-l TEXT` | `--library TEXT` | 使用するライブラリを指定する               |
| `add`                          | `-d`      | `--doc`          | プロジェクトを作らず、単一文書を作る       |
| `config add`                   | `-u TEXT` | `--url TEXT`     | ライブラリの Git URL を指定する            |
| `config add` / `config remove` | `-n TEXT` | `--name TEXT`    | ライブラリ名を指定する                     |

### 設定を初期化する

`reset` は `~/.typst_library_manager/config.json` が存在すれば削除します。その後、設定ディレクトリが空になった場合のみディレクトリ自体も削除します。もしディレクトリにほかのファイルが残っている場合は、安全のため削除せずにそのまま残します。取得済みの `libraries/` ディレクトリは削除しません。

```sh
tylm reset
```

このコマンドは冪等に設計されています。設定がすでに存在しないときに再度実行しても、失敗ではなく何もしない状態として扱います。

## 注意点

- ライブラリのエントリーファイルはリポジトリ直下に置いてください。`<name>.typ` または `<name>.typst` が優先され、なければ最初に見つかった `.typ`／`.typst` ファイルが使われます。
- 生成テンプレートはライブラリが `setup` を公開していることを想定しています。別の API のライブラリでは、作成後に生成ファイルを調整してください。
- ライブラリの更新は、必要に応じて `libraries/<name>/` 内で通常の Git 操作を行ってください。

## ライセンス

このプロジェクトは [MIT License](LICENSE) の下で提供されます。

同梱している CLI11 と JSON for Modern C++ の著作権表示およびライセンス全文は、[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) を参照してください。
