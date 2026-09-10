# Sistema de Gestão Agrícola

## Sobre o Projeto

O **Sistema de Gestão Agrícola** foi desenvolvido em linguagem C como projeto acadêmico, com o objetivo de auxiliar no gerenciamento e monitoramento de informações relacionadas a uma propriedade agrícola.

O sistema permite cadastrar talhões, registrar pragas e suas ocorrências, armazenar informações climáticas, analisar níveis de risco, consultar históricos, gerar alertas, emitir relatórios e visualizar graficamente os talhões da fazenda.

Os dados principais do sistema são armazenados em um banco de dados **SQLite**. O projeto também utiliza arquivos **CSV** e **TXT** para exportação, leitura e geração de relatórios.

A visualização gráfica da fazenda é realizada utilizando a biblioteca **Raylib**.

---

## Objetivo

O objetivo do sistema é centralizar informações agrícolas e permitir o acompanhamento das condições dos talhões através do cruzamento de dados relacionados a:

- Talhões;
- Plantações;
- Ocorrências de pragas;
- Níveis de infestação;
- Temperatura;
- Umidade;
- Histórico de registros;
- Classificação de risco.

A partir dessas informações, o sistema calcula o nível de risco de cada talhão e permite identificar áreas que necessitam de maior atenção.

---

## Tecnologias Utilizadas

O projeto utiliza:

- Linguagem C;
- SQLite;
- Biblioteca SQLite3;
- Raylib;
- GCC;
- Visual Studio Code;
- Arquivos CSV;
- Arquivos TXT.

---

## Estrutura do Projeto

O sistema foi dividido em módulos para facilitar sua organização e manutenção.

### main.c

Arquivo principal do sistema.

Responsável por:

- Abrir o banco de dados;
- Inicializar as tabelas;
- Exibir o menu principal;
- Direcionar o usuário para os diferentes módulos;
- Encerrar corretamente a conexão com o banco de dados.

### banco.c / banco.h

Responsável pela conexão e inicialização do banco de dados.

O banco utilizado pelo sistema é:

`agricola.db`

As principais tabelas são:

- `talhoes`;
- `pragas`;
- `ocorrencias_pragas`;
- `clima`.

Também são configuradas as chaves estrangeiras utilizadas para manter os relacionamentos entre os registros.

### menu.c / menu.h

Responsável pelos submenus do sistema.

Possui os menus de:

- Talhões;
- Pragas e ocorrências;
- Clima;
- Relatórios;
- Histórico.

Cada opção direciona o usuário para as funções correspondentes dos demais módulos.

### talhoes.c / talhoes.h

Responsável pelo gerenciamento dos talhões.

Permite:

- Cadastrar talhão;
- Listar talhões;
- Buscar talhão;
- Editar talhão;
- Excluir talhão.

Cada talhão possui:

- Código;
- Nome;
- Área;
- Tipo de plantação;
- Localização.

O sistema também realiza validações para evitar códigos duplicados e áreas inválidas.

### pragas.c / pragas.h

Responsável pelo gerenciamento das pragas e suas ocorrências.

Permite:

- Cadastrar pragas;
- Listar pragas;
- Buscar pragas;
- Editar pragas;
- Excluir pragas;
- Registrar ocorrências;
- Listar ocorrências;
- Editar ocorrências;
- Excluir ocorrências.

Cada praga possui um nível de risco entre **1 e 4**.

As ocorrências registram informações como:

- Código da ocorrência;
- Praga;
- Talhão;
- Nível de infestação;
- Área afetada;
- Data.

Antes de registrar uma ocorrência, o sistema verifica se a praga e o talhão informados existem.

A área afetada também é validada para impedir que seja maior que a área total do talhão.

### clima.c / clima.h

Responsável pelo gerenciamento dos dados climáticos.

Permite:

- Registrar dados climáticos;
- Listar o histórico climático;
- Visualizar a série climática de um talhão;
- Editar registros climáticos;
- Excluir registros climáticos.

Os registros armazenam:

- Talhão;
- Temperatura;
- Umidade;
- Data;
- Hora.

O módulo também possui funções utilizadas para calcular os fatores de risco relacionados à temperatura e à umidade.

### risco.c / risco.h

Responsável pela classificação de risco dos talhões.

O sistema considera três fatores:

- Risco de temperatura;
- Risco de umidade;
- Risco de praga.

O risco final é calculado através da multiplicação:

**Risco Final = Risco de Temperatura × Risco de Umidade × Risco de Praga**

Quando determinado fator não possui registros disponíveis, o sistema utiliza o valor **1** como valor padrão.

A classificação final utilizada é:

| Risco Final | Classificação |
|---|---|
| 1 a 6 | BAIXO |
| 7 a 16 | MÉDIO |
| 17 a 36 | ALTO |
| 37 a 64 | MUITO ALTO |

Para os dados climáticos é utilizado o registro mais recente do talhão.

Para as pragas é considerada a ocorrência mais recente e, quando existem múltiplas pragas registradas na mesma data, é utilizado o maior nível de risco.

### alertas.c / alertas.h

Responsável pela geração de alertas.

O módulo analisa os riscos dos talhões e identifica situações que necessitam de atenção.

Os alertas utilizam informações relacionadas a:

- Temperatura;
- Umidade;
- Pragas;
- Risco final.

Dessa forma, o usuário consegue identificar rapidamente talhões que apresentam condições de risco elevado.

### historico.c / historico.h

Responsável pela consulta do histórico dos registros.

Permite:

- Consultar o histórico geral;
- Consultar o histórico de um talhão específico;
- Visualizar a evolução do risco de um talhão.

Na evolução do risco, o sistema compara o risco atual com o registro anterior e informa se o risco:

- Aumentou;
- Diminuiu;
- Permaneceu estável.

Também são apresentados os fatores utilizados no cálculo do risco em cada período.

### relatorios.c / relatorios.h

Responsável pela geração e exportação de relatórios.

O módulo permite:

- Gerar relatório climático;
- Gerar relatório relacionando pragas e clima;
- Gerar resumo por período;
- Exportar os dados para CSV;
- Ler os arquivos CSV;
- Exportar relatório geral para TXT.

Os arquivos CSV gerados são:

- `talhoes.csv`;
- `clima.csv`;
- `pragas.csv`;
- `ocorrencias_pragas.csv`.

Também é gerado o arquivo:

`relatorio_fazenda.txt`

O relatório TXT reúne informações dos talhões, dados climáticos e ocorrências de pragas.

### visualizacao.c / visualizacao.h

Responsável pela visualização gráfica da fazenda utilizando **Raylib**.

Os talhões são apresentados em blocos organizados automaticamente de acordo com a quantidade de registros existentes.

Cada talhão apresenta:

- Código;
- Nome;
- Plantação;
- Área;
- Localização;
- Classificação de risco.

A cor do talhão representa seu nível de risco:

- **Verde:** risco baixo;
- **Amarelo:** risco médio;
- **Laranja:** risco alto;
- **Vermelho:** risco muito alto.

A quantidade de linhas e colunas da visualização é ajustada automaticamente de acordo com a quantidade de talhões cadastrados.

---

## Menu Principal

Ao iniciar o sistema, o seguinte menu é apresentado:

```text
====================================
          MENU PRINCIPAL
====================================
1 - Talhoes
2 - Pragas e Ocorrencias
3 - Clima
4 - Classificacao de Risco
5 - Relatorios
6 - Historico
7 - Alertas
8 - Visualizacao da Fazenda
0 - Sair
```

---

## Banco de Dados

O sistema utiliza **SQLite** para armazenamento persistente das informações.

Arquivo do banco:

```text
agricola.db
```

### Tabela talhoes

Armazena:

- Código;
- Nome;
- Área;
- Plantação;
- Localização.

### Tabela pragas

Armazena:

- Código;
- Nome;
- Descrição;
- Nível de risco.

### Tabela ocorrencias_pragas

Armazena:

- Código da ocorrência;
- Código da praga;
- Código do talhão;
- Nível de infestação;
- Área afetada;
- Data.

Possui relacionamento com as tabelas `pragas` e `talhoes`.

### Tabela clima

Armazena:

- ID;
- Código do talhão;
- Temperatura;
- Umidade;
- Data;
- Hora.

Possui relacionamento com a tabela `talhoes`.

---

## Validação e Tratamento de Dados

O sistema realiza diferentes validações antes de armazenar as informações.

Entre elas estão:

- Verificação de códigos duplicados;
- Verificação da existência de talhões;
- Verificação da existência de pragas;
- Validação de valores numéricos;
- Validação da área dos talhões;
- Validação da área afetada por uma praga;
- Validação do nível de infestação;
- Validação do nível de risco;
- Validação de datas;
- Verificação de registros antes de operações de edição e exclusão.

Essas validações ajudam a manter a consistência das informações armazenadas no sistema.

---

## Validação de Datas

As datas são utilizadas no formato:

```text
DD/MM/AAAA
```

Para realizar consultas cronológicas no SQLite, o sistema reorganiza internamente os componentes da data utilizando `substr()`.

Dessa forma:

```text
DD/MM/AAAA
```

é tratado durante determinadas consultas como:

```text
AAAA-MM-DD
```

Isso permite ordenar e comparar corretamente os registros por data sem alterar o formato apresentado ao usuário.

---

## Cálculo e Classificação de Risco

O sistema realiza o cruzamento das informações de clima e pragas para determinar o risco de cada talhão.

São considerados três fatores:

```text
Risco de Temperatura
Risco de Umidade
Risco de Praga
```

O cálculo utilizado é:

```text
Risco Final = Risco de Temperatura × Risco de Umidade × Risco de Praga
```

A classificação resultante é:

```text
1 a 6   -> BAIXO
7 a 16  -> MEDIO
17 a 36 -> ALTO
37 a 64 -> MUITO ALTO
```

Caso determinado fator não possua informações registradas, o valor inicial utilizado para esse fator é **1**.

---

## Histórico e Evolução do Risco

O sistema permite acompanhar o histórico das ocorrências registradas.

Também é possível consultar a evolução do risco de um talhão.

Durante essa análise, o sistema calcula o risco de cada período e compara com o registro anterior.

O resultado pode indicar:

```text
Situacao: risco aumentou
Situacao: risco diminuiu
Situacao: risco permaneceu estavel
Situacao: primeiro registro de risco
```

Essa funcionalidade permite acompanhar a evolução das condições de um talhão ao longo do tempo.

---

## Alertas

O módulo de alertas utiliza os fatores de risco calculados pelo sistema para identificar talhões que necessitam de atenção.

A análise considera:

```text
Temperatura
+
Umidade
+
Pragas
=
Risco do Talhao
```

Os alertas permitem destacar situações de risco elevado para facilitar a identificação de áreas críticas da fazenda.

---

## Relatórios

O sistema possui diferentes opções para consulta e geração de relatórios.

Entre elas:

- Relatório de clima;
- Relatório relacionando pragas e clima;
- Resumo por período;
- Exportação dos dados para CSV;
- Leitura dos arquivos CSV;
- Exportação de relatório geral para TXT.

O resumo por período permite consultar informações climáticas dentro de um intervalo de datas.

Entre as informações apresentadas estão:

- Quantidade de registros;
- Temperatura média;
- Umidade média;
- Maior temperatura;
- Menor temperatura.

---

## APS 1 - Ponteiros

O projeto utiliza ponteiros em diferentes partes do código.

Um dos exemplos utilizados especificamente para demonstrar o conteúdo da **APS 1** está no módulo de clima.

A função:

```c
void calcularRiscosClimaticos(float temperatura, float umidade, int *riscoTemperatura, int *riscoUmidade) {
    *riscoTemperatura = calcularRiscoTemperatura(temperatura);
    *riscoUmidade = calcularRiscoUmidade(umidade);
}
```

recebe dois ponteiros:

```c
int *riscoTemperatura
int *riscoUmidade
```

A chamada da função utiliza os endereços das variáveis:

```c
calcularRiscosClimaticos(temperatura, umidade, &riscoTemperatura, &riscoUmidade);
```

Dessa forma, uma única função consegue modificar duas variáveis externas e fornecer dois resultados para quem realizou a chamada.

Esse recurso demonstra o uso de ponteiros para retorno e manipulação de múltiplas variáveis em uma função.

---

## APS 2 - Manipulação de Arquivos

O projeto também implementa leitura e escrita de arquivos, atendendo aos requisitos da **APS 2**.

### Escrita em Arquivos

A escrita utiliza recursos da linguagem C como:

```c
FILE *
fopen()
fprintf()
fclose()
```

Exemplo simplificado:

```c
arquivo = fopen("talhoes.csv", "w");

fprintf(
    arquivo,
    "Codigo;Nome;Area;Plantacao;Localizacao\n"
);

fclose(arquivo);
```

O sistema exporta os dados para os seguintes arquivos:

```text
talhoes.csv
clima.csv
pragas.csv
ocorrencias_pragas.csv
```

Também existe exportação para:

```text
relatorio_fazenda.txt
```

### Leitura de Arquivos

A leitura dos arquivos CSV utiliza:

```c
fopen()
fgets()
fclose()
```

Exemplo:

```c
arquivo = fopen("talhoes.csv", "r");

while (fgets(linha, sizeof(linha), arquivo) != NULL) {
    printf("%s", linha);
}

fclose(arquivo);
```

Dessa forma, o sistema demonstra tanto a **gravação** quanto a **leitura** de informações armazenadas em arquivos.

---

## Visualização Gráfica com Raylib

A biblioteca **Raylib** é utilizada para gerar uma representação gráfica dos talhões da fazenda.

A janela gráfica possui dimensão:

```text
1200 x 750
```

A distribuição dos talhões é ajustada automaticamente de acordo com a quantidade cadastrada.

```text
Até 4 talhões  -> 2 colunas
Até 9 talhões  -> 3 colunas
Acima de 9     -> 4 colunas
```

A quantidade de linhas é calculada automaticamente.

Quando existe uma quantidade maior de linhas, o tamanho das fontes também é reduzido para permitir que as informações permaneçam dentro dos blocos.

Cada bloco representa um talhão e apresenta:

```text
Código
Nome
Plantação
Área
Localização
Risco
```

As cores utilizadas representam a classificação:

```text
Verde    -> BAIXO
Amarelo  -> MEDIO
Laranja  -> ALTO
Vermelho -> MUITO ALTO
```

Para sair da visualização gráfica e retornar ao sistema, o usuário pode pressionar **ESC**.

---

## Organização dos Arquivos

A estrutura principal do projeto é:

```text
Sistema Agricola/
|
|-- main.c
|
|-- alertas.c
|-- alertas.h
|
|-- banco.c
|-- banco.h
|
|-- clima.c
|-- clima.h
|
|-- historico.c
|-- historico.h
|
|-- menu.c
|-- menu.h
|
|-- pragas.c
|-- pragas.h
|
|-- relatorios.c
|-- relatorios.h
|
|-- risco.c
|-- risco.h
|
|-- talhoes.c
|-- talhoes.h
|
|-- visualizacao.c
|-- visualizacao.h
|
|-- agricola.db
|
|-- talhoes.csv
|-- clima.csv
|-- pragas.csv
|-- ocorrencias_pragas.csv
|-- relatorio_fazenda.txt
|
`-- .vscode/
    `-- tasks.json
```

---

## Compilação

O projeto utiliza **GCC** através do ambiente **MSYS2 UCRT64**.

Para compilar manualmente o sistema completo:

```bash
gcc main.c banco.c talhoes.c pragas.c clima.c relatorios.c menu.c risco.c alertas.c historico.c visualizacao.c -lsqlite3 -lraylib -lopengl32 -lgdi32 -lwinmm -o main.exe
```

Também existe uma configuração de compilação no Visual Studio Code através do arquivo:

```text
.vscode/tasks.json
```

No Visual Studio Code, o projeto pode ser compilado utilizando:

```text
Ctrl + Shift + B
```

Deve ser utilizada a tarefa responsável pela compilação do **sistema completo**, pois os diferentes módulos possuem dependências entre si.

---

## Execução

Após a compilação, execute:

```bash
./main.exe
```

No Windows também pode ser utilizado:

```bash
main.exe
```

Ao iniciar, o sistema abre o arquivo:

```text
agricola.db
```

Em seguida, as tabelas necessárias são inicializadas caso ainda não existam.

Após a inicialização, o menu principal é apresentado ao usuário.

---

## Fluxo Básico de Utilização

Uma sequência básica para utilização do sistema é:

```text
1. Iniciar o sistema
        |
        v
2. Cadastrar um talhão
        |
        v
3. Cadastrar uma praga
        |
        v
4. Registrar uma ocorrência de praga
        |
        v
5. Registrar dados climáticos
        |
        v
6. Consultar a classificação de risco
        |
        v
7. Consultar alertas e histórico
        |
        v
8. Gerar relatórios
        |
        v
9. Visualizar graficamente a fazenda
```

Os módulos podem ser utilizados independentemente quando os dados necessários já estiverem cadastrados.

---

## Principais Funcionalidades

O sistema atualmente possui:

- Cadastro de talhões;
- Consulta de talhões;
- Edição de talhões;
- Exclusão de talhões;
- Cadastro de pragas;
- Consulta de pragas;
- Edição de pragas;
- Exclusão de pragas;
- Cadastro de ocorrências de pragas;
- Consulta de ocorrências;
- Edição de ocorrências;
- Exclusão de ocorrências;
- Registro de informações climáticas;
- Consulta do histórico climático;
- Consulta da série climática por talhão;
- Edição de registros climáticos;
- Exclusão de registros climáticos;
- Validação da existência de talhões;
- Validação da existência de pragas;
- Validação da área afetada;
- Validação de datas;
- Classificação automática de risco;
- Cruzamento de informações de clima e pragas;
- Geração de alertas;
- Consulta do histórico geral;
- Consulta do histórico por talhão;
- Análise da evolução do risco;
- Relatórios climáticos;
- Relatórios relacionando clima e pragas;
- Resumo climático por período;
- Exportação dos dados para arquivos CSV;
- Leitura dos arquivos CSV;
- Exportação de relatório geral em TXT;
- Persistência das informações utilizando SQLite;
- Utilização de ponteiros;
- Manipulação de arquivos;
- Visualização gráfica da fazenda utilizando Raylib;
- Representação visual dos níveis de risco através de cores.

---

## Conceitos Aplicados no Projeto

Durante o desenvolvimento foram utilizados diferentes conceitos da linguagem C e de desenvolvimento de software, entre eles:

- Variáveis e tipos de dados;
- Estruturas condicionais;
- Estruturas de repetição;
- Funções;
- Modularização;
- Arquivos `.c` e `.h`;
- Structs;
- Ponteiros;
- Arrays e strings;
- Manipulação de arquivos;
- Entrada e saída de dados;
- Validação de informações;
- Banco de dados;
- SQL;
- Chaves primárias;
- Chaves estrangeiras;
- Relacionamentos entre tabelas;
- Consultas utilizando JOIN;
- Biblioteca gráfica;
- Organização modular do código.

---

## Considerações Finais

O **Sistema de Gestão Agrícola** integra diferentes conceitos estudados durante o desenvolvimento do projeto, combinando programação em C, modularização, estruturas, funções, ponteiros, manipulação de arquivos, banco de dados SQLite, consultas SQL e utilização da biblioteca gráfica Raylib.

A divisão do sistema em módulos permite separar as responsabilidades de cada funcionalidade, tornando o código mais organizado e facilitando sua manutenção e evolução.

Além do armazenamento das informações, o sistema utiliza os registros de clima e pragas para gerar classificações de risco, históricos, alertas e relatórios.

A visualização gráfica complementa essas funcionalidades ao representar os talhões e seus respectivos níveis de risco através de cores, permitindo uma interpretação mais rápida das condições da propriedade agrícola.

O projeto demonstra, dessa forma, a integração entre **programação em C, banco de dados, manipulação de arquivos, análise de informações e interface gráfica** em uma única aplicação.