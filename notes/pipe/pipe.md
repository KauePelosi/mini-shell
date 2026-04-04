# Pipe

## O que é:
Dentro de um shell a pipe é uma estrutura que conecta a saída de um comando a entrada de outro.

### Exemplo:

```
ls -l | grep ".cpp"
```

O comando "ls -l" lista os arquivos de determinado diretório com mais detalhes, a partir disso a pipe redireciona essa saída de dados como entrada para o comando "grep .cpp" que faz com que sejam listados apenas os arquivos com a extensão .cpp daquele diretório.

## Objetivo e utilidade
O pipe nasce de alguns conceitos do Unix que dizem:
"Faça uma coisa e a faça bem".
Dessa forma o real poder e utilidade do pipe é a praticidade que ele te da ao permitir que comandos pequenos se conectem entre si de forma extremamente rápida e organizada.
