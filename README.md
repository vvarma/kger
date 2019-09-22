## KGER 

### Summary
DBPedia has a rich triplet dataset that's sourced from wikipedia based on different parsers provided in their extraction framework. Currently, this project is built towards identifying the entity type from the short abstract associated with each wikipedia page. The dataset used was sourced from:
https://wiki.dbpedia.org/downloads-2016-10#p10608-2

### Approach
- The model uses a LSTM layer to extract features for the multiclass classifier
- The dataset used maintains an in memory graph object which contains Named-Entites, Instances and Tokenized text nodes.
- The model is trained to predict the correct Named-Entity type given the Tokenized text.
- Text tokenization is done using a sentencepiece model that was trained from scratch on the same text data.
- The code uses libTorch - the c++ API for PyTorch

The model gives an accuracy of

### Future work
- The hierarchial nature of the ontology is not captured well with a multi-class classifier. A multi-label classifier is more suitable.
- The model uses the first n tokens of the short abstract associated with a wikipedia page to identify the type. Instead, it should be able to work on the raw wiki text data itself.
