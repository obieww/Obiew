import MainPanel from '@/components/body/MainPanel'
import { connect } from 'react-redux'
import {
  changeUser,
  createNewObiew,
  createNewComment,
  createNewLike,
  deleteLike
} from '@/store/actions'

const mapStateToProps = state => ({
  username: state.username,
  obiews: state.obiews
})

const mapDispatchToProps = dispatch => ({
  onChangeUser: username => 
    dispatch(changeUser(username)),

  onCreateNewObiew: obiew => 
    dispatch(createNewObiew(obiew)),

  onCreateNewComment: (obiewId, comment) => 
    dispatch(createNewComment(obiewId, comment)),

  onCreateNewLike: (obiewId, like) =>
    dispatch(createNewLike(obiewId, like)),

  onDeleteLike: (obiewId, like) =>
    dispatch(deleteLike(obiewId, like)),
})

export default connect(mapStateToProps, mapDispatchToProps)(MainPanel)
