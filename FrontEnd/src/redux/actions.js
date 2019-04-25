const SET_CARD_ID = "setCardId"

function setCardId(cardId) {
    return {
        type: SET_CARD_ID,
        cardId: cardId
    };
}

export {SET_CARD_ID, setCardId}